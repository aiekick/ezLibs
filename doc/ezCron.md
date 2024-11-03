# EzCron

EzCron is a one header file only for parse 
Crontab rule and check time for act

# Supported Crontab rule format 

```
'mm hh dd MM DD'
  mm : the minutes from 0 to 59
  hh : the hour from 0 to 23
  dd : the day of the month from 1 to 31
  MM : the month from 1 to 12
  DD : the day of the week from 0 to 7. 0 and 7 are the sunday

For each fields, thoses forms are accepted :
 * : always valid units (0,1,3,4, etc..)
 5,8 : the units 5 and 8
 2-5 : the units from 2 to 5 (2, 3, 4, 5)
 */3 : all the 3 interval units(0, 3, 6, 9, etc..)
```

# How to use

Simple use

```cpp
#include <iostream>
#include <EzTools/ezCron.hpp>

int main() {
	ez::time::Cron cr("* */2 * * *"); // each 2 hours
	if (cr.isTimeToAct()) {
		// do action
	}
	if (cr.isTimeToAct(epoch_time)) {
		// do action
	}
	return 0;
}

```

EzCron comme with a detailed way for get and locate format errors:

this bad rule :

```cpp
ez::time::Cron cr("62 28 35 32 68 12");
std::cout << cr.getErrorMessage() << std::endl;
```

will give this output :

```
62 28 35 32 68 12
|  |  |  |  |  ^--<|  Invalid fields count.
|  |  |  |  ^--<|  Invalid week day.
|  |  |  ^--<|  Invalid month.
|  |  ^--<|  Invalid month day.
|  ^--<|  Invalid hour.
^--<|  Invalid minute.
```

the function getSupportedFormat() will give :

```
Supported Crontab format
 mm hh dd MM DD    
 mm : the minutes from 0 to 59
 hh : the hour from 0 to 23
 dd : the day of the month from 1 to 31
 MM : the month from 1 to 12
 DD : the day of the week from 0 to 7. 0 and 7 are the sunday
For each fields, thoses forms are accepted :
 * : always valid units (0,1,3,4, etc..)
 5,8 : the units 5 and 8
 2-5 : the units from 2 to 5 (2, 3, 4, 5)
 */3 : all the 3 interval units (0, 3, 6, 9, etc..)
```
