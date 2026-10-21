"""Small independent reference images; never invokes image_processor."""
import math
from PIL import Image


def apply_filter(rows, name, params):
    height, width = len(rows), len(rows[0])

    def pixel(x, y):
        return rows[max(0, min(y, height - 1))][max(0, min(x, width - 1))]

    if name == 'crop':
        return [row[:int(params[0])] for row in rows[:int(params[1])]]
    if name == 'gs':
        return [[(0.299 * r + 0.587 * g + 0.114 * b,) * 3 for r, g, b in row] for row in rows]
    if name == 'neg':
        return [[tuple(1 - c for c in p) for p in row] for row in rows]
    if name == 'edge':
        rows = apply_filter(rows, 'gs', [])
    if name in ('sharp', 'edge'):
        result = []
        for y in range(height):
            row = []
            for x in range(width):
                values = [(5 if name == 'sharp' else 4) * pixel(x, y)[c]
                          - sum(pixel(nx, ny)[c] for nx, ny in
                                [(x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)])
                          for c in range(3)]
                row.append(tuple(max(0, min(1, c)) for c in values) if name == 'sharp'
                           else (float(values[0] > float(params[0])),) * 3)
            result.append(row)
        return result
    if name == 'blur':
        sigma = float(params[0])
        if sigma == 0:
            return rows
        radius = math.ceil(3 * sigma)
        # Direct 2D convolution, independent of the C++ two-pass algorithm.
        kernel = [(dx, dy, math.exp(-(dx * dx + dy * dy) / (2 * sigma * sigma)))
                  for dy in range(-radius, radius + 1) for dx in range(-radius, radius + 1)]
        total = sum(weight for _, _, weight in kernel)
        return [[tuple(sum(pixel(x + dx, y + dy)[c] * weight
                           for dx, dy, weight in kernel) / total for c in range(3))
                 for x in range(width)] for y in range(height)]
    raise ValueError(name)


def save_image(path, rows):
    image = Image.new('RGB', (len(rows[0]), len(rows)))
    image.putdata([tuple(math.floor(max(0, min(1, c)) * 255 + 0.5) for c in pixel)
                   for row in rows for pixel in row])
    image.save(path, format='BMP')


def generate_cases(directory, case_type):
    cases = []
    sources = {
        'colors': [[(x / 4, y / 3, ((x + 2 * y) % 5) / 4) for x in range(5)] for y in range(4)],
        'single': [[(0.2, 0.4, 0.8)]],
        'row': [[(0.2, 0.2, 0.2), (0.4, 0.4, 0.4), (0.8, 0.8, 0.8)]],
        'column': [[(0.2, 0.4, 0.8)], [(0.8, 0.4, 0.2)]],
        'white': [[(1, 1, 1)] * 5 for _ in range(4)],
    }
    scenarios = [([], 'copy'), (['-crop', '2', '3'], 'crop'),
                 (['-crop', '999', '1999'], 'crop_large'),
                 (['-crop', '999', '1999', '-crop', '100', '1'], 'crop_crop'),
                 (['-crop', '1', '999'], 'crop_narrow'),
                 (['-gs'], 'gs'), (['-gs', '-gs'], 'gs_gs'),
                 (['-neg'], 'neg'), (['-neg', '-neg'], 'neg_neg'),
                 (['-sharp'], 'sharp'), (['-sharp', '-sharp'], 'sharp_sharp'),
                 (['-edge', '0.1'], 'edge'), (['-edge', '0.1', '-edge', '0.5'], 'edge_edge'),
                 (['-edge', '0'], 'edge_zero'), (['-blur', '0'], 'blur_zero'),
                 (['-blur', '0.5'], 'blur'),
                 (['-crop', '2', '1', '-sharp'], 'crop_sharp'),
                 (['-sharp', '-crop', '2', '1'], 'sharp_crop'),
                 (['-crop', '3', '2', '-gs', '-blur', '0.5'], 'chain')]
    for source, rows in sources.items():
        # Reference filters operate on the exact quantized input bytes.
        rows = [[tuple(math.floor(c * 255 + 0.5) / 255 for c in p) for p in row] for row in rows]
        save_image(directory / f'{source}.bmp', rows)
        for args, name in scenarios:
            expected = rows
            index = 0
            while index < len(args):
                filter_name = args[index][1:]
                count = {'crop': 2, 'edge': 1, 'blur': 1}.get(filter_name, 0)
                expected = apply_filter(expected, filter_name, args[index + 1:index + 1 + count])
                index += 1 + count
            save_image(directory / f'{source}_{name}.bmp', expected)
            cases.append(case_type(name=name, input=source, args=args, eps=0.0))
    # Same sigma values as the original example, on a small independent image.
    for name, args in [('blur_large', ['-blur', '7.5']),
                       ('blur_blur', ['-blur', '7.5', '-blur', '3'])]:
        rows = [[tuple(math.floor(c * 255 + 0.5) / 255 for c in pixel) for pixel in row]
                for row in sources['colors']]
        expected = apply_filter(rows, 'blur', ['7.5'])
        if name == 'blur_blur':
            expected = apply_filter(expected, 'blur', ['3'])
        save_image(directory / f'colors_{name}.bmp', expected)
        cases.append(case_type(name=name, input='colors', args=args, eps=1.0))
    # White is invariant for the custom filter, including its white border fill.
    save_image(directory / 'white_glass.bmp', sources['white'])
    cases.append(case_type(name='glass', input='white', args=['-glass', '2', '0.5'], eps=0.0))
    return cases
