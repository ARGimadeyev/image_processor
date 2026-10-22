# Запуск тестов

Тесты используют исходный `ImageProcessorTester` и `TestCase(input, name, args, eps)` из примера.

Нужны Python 3 и Pillow (`python3 -m pip install Pillow`).

Из корня проекта:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build -j 2
python3 test_script/test_image_processor.py ./build/image_processor
```

Либо через CTest:

```sh
ctest --test-dir build --output-on-failure
```

Пути к эталонам определяются относительно скрипта, поэтому тесты можно запускать из другой папки.

## Проверки

- Все 16 исходных TestCase сохранены. Шесть проверок `flag` есть.
- + 98 проверок изображений: базовые фильтры, повторные применения, порядок фильтров, crop за границами, одиночный пиксель, одна строка/колонка, размытие с разными sigma, сохранение белого изображения фильтром glass.
- + 35 проверок справки, неверных аргументов, ошибок файлов и повреждённых BMP. Завершение по сигналу считается провалом, для обработанной ошибки требуется сообщение в stderr.

Дополнительные эталоны создаются `reference_images.py`.