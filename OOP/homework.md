# Домашни

## Домашно 1
Напишете програма, която ще представлява база от данни за притежатели на коли, съхранявайки следните данни:
 - Име на притежателя (не по дълго от 23 символа)
 - Униканлен идентификационен номер на колата
 - Регистрационен номер на колата (неотрицателно четирицифрено число)

Колите, които могат да бъдат притежавани от хората, са:

| име                    | идентификационен номер    | мощност   |
|------------------------|---------------------------|-----------|
| Lambordgini Murcielago | 0                         | 670       |
| Mercedes-AMG           | 1                         | 503       |
| Pagani Zonda R         | 2                         | 740       |
| Bugatti Veyron         | 3                         | 1020      |

Вашата програма трябва да поддържа следните функции:
 - Добавяне на нов запис за притежател на кола с име, регистрационен номер на колата и идентификационен номер на колата. Ако в програмата съществува запис за кола със същия регистрационен номер, извеждате съобщение за грешка и не добавяте записа.
 - Генериране на текстов файл с име "car-report.txt" и следната информация:
  - Най-популярна кола - най-много хора я притежават.
  - Средна мощност на всички притежавани коли.
  - По един ред за всеки притежател с името и общата мощност на притежаваните от него коли.
 - Генериране на бинарен файл с име "db-save.dat", в който седят записи за всички притежатели на коли. Ако файлът вече съществува, се презаписва.
 - При пускане, да зарежда притежатели на коли от бинарен файл с име "db-save.dat", ако такъв не съществува все едно е бил празен.

### Забележки:
 * Един човек, може да притежава повече от една кола - за програмата един и същи човек е такъв, който присъства няколко пъти с еднакво име.