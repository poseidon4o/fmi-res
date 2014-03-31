## Проекти/Домашни за ООП практикум ИС 2013/2014 ##
---
### Проект 1: Command Line Utilities - *нечетни номера*

Този проект представлява серия от инструменти с които ще се обработват текстови и бинарни файлове. Обработката на текстовите файловете ще включва филтриране на съдържанието по дадени критерии, търсене и сортиране. Ще има включени инструменти за четене и писане във файлове по зададени параметри. Инструментите ще са подходящо направени, така че да позволяват използването на няколко от тях едновременно върху едни данни (файл). Например прочитане на текстов файл и извеждане на всички редове от него, които съдържат определен низ, след което записване на получения резултат в друг файл или извеждане на конзолата.


**Домашно 1.** Напишете клас представящ филтър, който предоставя възможността да се чете вход от конзолата до край на файл ([ctrl] + D/Z). След това може да изведе последователно тези от прочетените редове, в които се съдържа избрана дума. Преценете какви член функции и член данни трябва да има този клас. Демонстрирайте използването му в кратка main функция.

**Домашно 2.** Напишете клас представящ подредена поредица от филтри (FilterChain). Предефинирайте подходящите оператори за целта. Класът ще трябва да може се инициализира с поток за писане и поток за четене. Класът предоставя възможността да запише филтрираното съдържание от входния поток, в изходния поток. Добавете подходящи методи за добавяне и премахване на филтрите от класа. Този клас трябва да може да се записва (сериализира) и чете (десериализира) от бинарен файл.

---

### Проект 2: Игра с карти - *четни номера*

Целта на проектa ви е да реализирате походова игра с карти. Играта се играе от двама играчи, всеки от които притежава предварително зададено тесте. В тестето участват до 10 карти. Всяка карта притежава четири свойства - Атака, Живот, Необходима енергия, Умения (от 1 до 3 на брой). Победител се излъчва когато единия играч остане без карти. За изваждането на карта на "бойното поле" е необходима енергия. Играчите започват с опередена енергия и след всеки ход получават допълнителна такава. 

**Домашно 1.** Първата ви задача около този проект е да създадете клас карта. Той трябва да притежава горепосочените свойства като на този етап ще игнорираме уменията.
Необходимата енергия се пресмята по формулата Е = Ж/100 + А/20 (Е - необходима енергия, Ж - живот, А - атака). Класът трябва да притежава конструктор, копиращ конструктор
и деструктор. Останалите методи и член-данни трябва да бъдат избрани от вас. Демонстрирайте използването на този клас в кратка main функция.

**Домашно 2.** Имплементирайте нов клас Deck (тесте), които да съдържа масив от карти (произволен брой). За да кажем че тестето е валидно то трябва да съдържа поне 5 карти. Класа Deck трябва да има методи които му позволяват да се записва/инициализира във/от файл (бинарен), ако тестето е валидно. Освен това трябва да поддържате добавяне на нова, изтриване или промяна на съществуваща карта от тестето (респективно от файла). Същото така трябва да имате функция, която да записва 5-те най-добри карти по даден критерии в текстов файл (критерия ще се получава като аргумент на функцията). Ако е нужно да се допълни класа карта с нови параметри и функции. Документирайте кода по подходящ начин (коментари + описания).
