# ImgConverter - Редактор изображений
## Функции:
* Конвертирование изображения из одного типа в другой. Поддержка типов файлов ppm, jpeg, bmp.
* Отражение по вертикали
* Отражение по горизонтали
* Создание негатива
* Обработка оператором Собеля
* Шифрование изображения

## Сборка с помощью CMake
1.	Для поддержки файлов jpeg используется библиотека LibJPEG `https://www.ijg.org/`\
В папке libjpeg собрана библиотека под Windows при помощи MinGW.
2.	Создайте папку для сборки программы
3.	В консоли перейдите в созданный каталог и введите команду:\
`cmake <путь к файлу CMakeLists.txt> -DCMAKE_BUILD_TYPE=Debug -DLIBJPEG_DIR=<путь к собранной библиотеке libjpeg>`\
По необходимости следует указать ключ с компилятором.
Например: `-G "Eclipse CDT4 - MinGW Makefiles"`
4.	Введите команду: `cmake --build . `
5.	После успешной сборки в каталоге для сборки программы появится выполняемый файл imgconv.exe

## Использование собранной версии программы
Программа запускается в консоли с ключами:\
`./imgconv.exe <in_file> <out_file> [convert|hmirror|vmirror|sobel|negate|cypher <key>]`

* convert – конвертирование изображения из одного типа в другой. Поддержка ppm, jpeg, bmp.\
Пример: `./imgconv.exe cat.bmp cat.jpg covert`
![cat](https://user-images.githubusercontent.com/88826237/174062899-8618411b-3b35-457b-a1f3-39c7c94ead9d.jpg)
* hmirror – отражение изображения по горизонтали и сохранение результата в любой тип файла\
Пример: `./imgconv.exe cat.bmp cat.jpg hmirror`
![cat_hmirror](https://user-images.githubusercontent.com/88826237/174062926-d6bb1d17-0a52-4d7b-b750-824dcf7489ea.jpg)
* vmirror – отражение изображения по вертикали и сохранение результата в любой тип файла\
Пример: `./imgconv.exe cat.bmp cat.jpg vmirror`
![cat_vmirror](https://user-images.githubusercontent.com/88826237/174062954-7002f9aa-d4a9-40c4-98d7-4f774ca7ed4c.jpg)
* sobel – обработка изображения оператором собеля и сохранение результата в любой тип файла\
Пример: `./imgconv.exe cat.bmp cat.jpg sobel`
![cat_cobel](https://user-images.githubusercontent.com/88826237/174062971-ca4e2f86-d899-4c10-85c3-525994aa280d.jpg)
* negate – создание негатива изображения и сохранение результата в любой тип файла\
Пример: `./imgconv.exe cat.bmp cat.jpg negate`
![cat_negate](https://user-images.githubusercontent.com/88826237/174062989-61be420e-a8e1-4f05-b1fc-30480844c327.jpg)
* cypher <key> – шифрование изображения с ключом шифрования и сохранение результата в любой тип файла. 
 Расшифровка ранее зашифрованного изображения производится повторным вызовом программы с тем же ключом шифрования\
Пример: `./imgconv.exe cat.bmp cat.jpg cypher 123`
  ![cat_cypher](https://user-images.githubusercontent.com/88826237/174063010-9643c0cf-ee80-4972-a6ed-7ec4410e2be7.jpg)

