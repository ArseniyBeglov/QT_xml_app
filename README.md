# QT_xml_app

Разработать программу для обработки xml документов определенной структуры. Пример обрабатываемого файла - input.xml. При выборе файла программа создает DOM модель файла и выводит ее состав в текстовое поле. Далее у пользователя должно быть три возможности:
1.	Удалить Item с определенным PartNumber. Выбор PartNumber сделать через combobox.
2.	Добавить в обрабатываемый файл Item с определенным PartNumber. Пользователь указывает PartNumber, программа при помощи QXmlStreamReader или QXmlSimpleReader ищет Item с данным PartNumber в файле ItemLibrary.xml. Если Item найден, то он добавляется в обрабатываемый файл, иначе выводится сообщение об ошибке.
3.	Сохранить обрабатываемый файл. 
