#!/usr/bin/python3

from collections import namedtuple
from PIL import Image, ImageChops, UnidentifiedImageError
import math
from pathlib import Path
import struct
from reference_images import generate_cases
import subprocess
import sys
import tempfile


def calc_images_distance(image_path1, image_path2):
    with Image.open(image_path1) as image1, Image.open(image_path2) as image2:
        if image1.size != image2.size or image1.mode != "RGB" or image2.mode != "RGB":
            return math.inf
        histogram = ImageChops.difference(image1, image2).histogram()
        return math.sqrt(sum(count * (index % 256) ** 2 for index, count in enumerate(histogram))
                         / (3 * image1.width * image1.height))



class ImageProcessorTester:
    TestCase = namedtuple("TestCase", ["name", "input", "args", "eps"])

    class TestCaseFailedException(Exception):
        pass

    def __init__(self, image_processor_executable):
        self.image_processor_executable = str(Path(image_processor_executable).resolve())
        self.data_dir = Path(__file__).resolve().parent / "data"

    @staticmethod
    def fail_test_case(input, name, message):
        print("FAIL [{input}_{name}] {message}".format(input=input, name=name, message=message))
        raise ImageProcessorTester.TestCaseFailedException()

    @staticmethod
    def succeed_test_case(input, name):
        print("OK   [{input}_{name}]".format(input=input, name=name))

    def run_tests(self):

        filter_test_cases = {
            "crop": [
                ImageProcessorTester.TestCase(input="lenna", name="crop", args=["-crop", "999", "1999"], eps=0.0),
                ImageProcessorTester.TestCase(input="lenna", name="crop_crop",
                                              args=["-crop", "999", "1999", "-crop", "100", "1"],
                                              eps=0.0),
                ImageProcessorTester.TestCase(input="flag", name="crop", args=["-crop", "50", "50"], eps=0.0),
            ],
            "edge": [
                ImageProcessorTester.TestCase(input="flag", name="edge", args=["-edge", "0.1"], eps=1.0),
                ImageProcessorTester.TestCase(input="flag", name="edge_edge", args=["-edge", "0.1", "-edge", "0.5"],
                                              eps=1.0),
            ],
            "gs": [
                ImageProcessorTester.TestCase(input="lenna", name="gs", args=["-gs"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="gs_gs", args=["-gs", "-gs"], eps=1.0),
                ImageProcessorTester.TestCase(input="flag", name="gs", args=["-gs"], eps=1.0),
            ],
            "neg": [
                ImageProcessorTester.TestCase(input="lenna", name="neg", args=["-neg"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="neg_neg", args=["-neg", "-neg"], eps=1.0),
                ImageProcessorTester.TestCase(input="flag", name="neg", args=["-neg"], eps=1.0),
            ],
            "sharp": [
                ImageProcessorTester.TestCase(input="lenna", name="sharp", args=["-sharp"], eps=1.0),
                ImageProcessorTester.TestCase(input="lenna", name="sharp_sharp", args=["-sharp", "-sharp"], eps=1.0),
                ImageProcessorTester.TestCase(input="flag", name="sharp", args=["-sharp"], eps=1.0)
            ],
            "blur": [
                ImageProcessorTester.TestCase(input="lenna", name="blur", args=["-blur", "7.5"], eps=2.0),
                ImageProcessorTester.TestCase(input="lenna", name="blur_blur", args=["-blur", "7.5", "-blur", "3"],
                                              eps=2.0),
            ],
        }
        passed = failed = skipped = 0
        for test_cases in filter_test_cases.values():
            for test_case in test_cases:
                required = [self.data_dir / f"{test_case.input}.bmp",
                            self.data_dir / f"{test_case.input}_{test_case.name}.bmp"]
                missing = [path.name for path in required if not path.is_file()]
                if missing:
                    print(f"SKIP [{test_case.input}_{test_case.name}] missing: {', '.join(missing)}")
                    skipped += 1
                    continue
                try:
                    self.run_test_case(test_case)
                    passed += 1
                except self.TestCaseFailedException:
                    failed += 1

        with tempfile.TemporaryDirectory(prefix="image_processor_tests_") as directory:
            generated_dir = Path(directory)
            for test_case in generate_cases(generated_dir, self.TestCase):
                try:
                    self.run_test_case(test_case, generated_dir)
                    passed += 1
                except self.TestCaseFailedException:
                    failed += 1
            for name, args, expected_success in self.command_cases(generated_dir):
                try:
                    result = subprocess.run([self.image_processor_executable] + list(map(str, args)),
                                            capture_output=True, timeout=10)
                    if expected_success:
                        valid = result.returncode == 0 and b"image_processor" in result.stdout
                    else:
                        valid = result.returncode > 0 and bool(result.stderr.strip())
                    if not valid:
                        self.fail_test_case("cli", name, f"unexpected result: {result.returncode}, {result.stderr!r}")
                    self.succeed_test_case("cli", name)
                    passed += 1
                except subprocess.TimeoutExpired:
                    print(f"FAIL [cli_{name}] timeout")
                    failed += 1
                except self.TestCaseFailedException:
                    failed += 1
        print(f"-----\nPASSED: {passed}, FAILED: {failed}, SKIPPED: {skipped}\n-----")
        return failed == 0

    def command_cases(self, directory):
        source = directory / "colors.bmp"
        output = directory / "output.bmp"
        yield "help", [], True
        yield "help_option", ["--help"], True
        yield "missing_output", [source], False
        yield "missing_input", [directory / "missing.bmp", output], False
        yield "invalid_output", [source, directory / "missing" / "output.bmp"], False
        invalid = [["-unknown"], ["-gs", "1"], ["-neg", "1"], ["-sharp", "1"],
                   ["-crop"], ["-crop", "0", "1"], ["-crop", "-1", "2"],
                   ["-crop", "1.5", "2"], ["-crop", "999999999999999999999", "1"],
                   ["-edge"], ["-edge", "nan"], ["-edge", "0.1junk"],
                   ["-blur", "-1"], ["-blur", "inf"], ["-blur", "1e100"],
                   ["-glass", "1"], ["-glass", "2", "-1"], ["oops"]]
        for index, args in enumerate(invalid):
            yield f"invalid_args_{index}", [source, output] + args, False
        original = source.read_bytes()
        damaged = directory / "damaged.bmp"
        for name, data in [("empty", b""), ("signature", b"XX" + original[2:]),
                           ("header", original[:30]), ("pixels", original[:-1])]:
            damaged.write_bytes(data)
            yield name, [damaged, output], False
        for name, offset, fmt, value in [
                ("dib", 14, "<I", 108), ("bits", 28, "<H", 32),
                ("compression", 30, "<I", 1), ("width", 18, "<i", 0),
                ("height", 22, "<i", 0), ("offset", 10, "<I", 20),
                ("planes", 26, "<H", 2), ("palette", 46, "<I", 1)]:
            data = bytearray(original)
            struct.pack_into(fmt, data, offset, value)
            damaged.write_bytes(data)
            yield name, [damaged, output], False

    def run_test_case(self, test_case, data_dir=None):
        data_dir = self.data_dir if data_dir is None else data_dir
        try:
            input_file_name = "{input}.bmp".format(input=test_case.input)
            output_file_name = "{input}_{name}.bmp".format(input=test_case.input, name=test_case.name)
            input_file = str(data_dir / input_file_name)
            expected_output_file = str(data_dir / output_file_name)

            with tempfile.NamedTemporaryFile(suffix=".bmp") as output_file:
                subprocess.check_call([self.image_processor_executable, input_file, output_file.name] + test_case.args,
                                      timeout=180)

                images_distance = calc_images_distance(expected_output_file, output_file.name)
                if images_distance > test_case.eps:
                    self.fail_test_case(test_case.input, test_case.name,
                                        "output image differs from expected with rms diff {diff}".format(
                                            diff=images_distance))

            self.succeed_test_case(test_case.input, test_case.name)
        except subprocess.CalledProcessError:
            self.fail_test_case(test_case.input, test_case.name, "image_processor finished with non-zero exit code")
        except subprocess.TimeoutExpired:
            self.fail_test_case(test_case.input, test_case.name, "timeout")
        except FileNotFoundError:
            self.fail_test_case(test_case.input, test_case.name, "output file not found")
        except (UnidentifiedImageError, OSError, ValueError):
            self.fail_test_case(test_case.input, test_case.name, "output file is corrupt")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit("Usage: python3 test_image_processor.py /path/to/image_processor")
    tester = ImageProcessorTester(image_processor_executable=sys.argv[1])

    print(
        "Running image_processor tests\nExecutable: {executable}".format(executable=tester.image_processor_executable))

    if not tester.run_tests():
        sys.exit(1)
