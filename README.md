# Typography
## Описание
Приложение для ведения базы заказов типографии на платформе Windows.
Программа работает с базой данных PostgreSQL. 
Основная задача - оформление и контроль заказов на изготовление печатной продукции и расчет их стоимости.
Реализована авторизация пользователей.
### Возможности
1. Оформление заказа на изготовление печатной продукции.
2. Редактирование созданных заказов.
3. Постановка заказа в очередь на исполнение.
4. Удаление / восстановление удаленных заказов.
5. Фильтрация заказов по определенным параметрам.
6. Контроль состояния заказа в производственном процессе.
7. Ведение базы данных заказчиков.
8. Обеспечена авторизация пользователей с контролем прав.
## Графический интерфейс
Реализован оконный графический интерфейс приложения с использованием фреймворка QT6.
![Alt text](https://github.com/a-schus/Typography/blob/main/media/2023-09-29_13-42-25.png?raw=true)
![Alt text](https://github.com/a-schus/Typography/blob/main/media/2023-09-29_13-39-20.png?raw=true)
![Alt text](https://github.com/a-schus/Typography/blob/main/media/2023-09-29_13-40-03.png?raw=true)
![Alt text](https://github.com/a-schus/Typography/blob/main/media/2023-09-29_13-40-40.png?raw=true)
![Alt text](https://github.com/a-schus/Typography/blob/main/media/2023-09-29_13-41-48.png?raw=true)
## Инструменты
Для реализации приложения использованы: 
1. Свободная объектно-реляционная система управления базами данных PostgreSQL.
2. Язык программирования C++11.
3. Фреймворк QT6.
4. Система сборки QMake.

Была разработана структура базы данных заказов для печати, использующая систему управления базами данных PostgreSQL.
Для разработки графического интерфейса и работы с базой данных был выбран фреймворк QT.

## Запуск приложения
Для корректной работы приложения требуется установить систему управления базами данных PostgreSQL и загрузить в нее дамп базы данных Typography из папки DB/.
Для первого входа в программу используйте учетную запись Administrator, пароль вводить не нужно. После успешного входа нужно перейти в меню Инструменты->Пользователи->Изменить пароль и установить пароль для учетной зписи администратора.
