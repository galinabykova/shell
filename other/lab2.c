#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
extern char *tzname[];
int main() {
	//putenv("TZ=America/Los_Angeles");//EST5EDT
	//putenv("TZ=PST8PDT");//PST8PDT Pacific Standart Time, 8 часов на запад от Гринвича, с переводом летнего/зимнего времени из презентации
	putenv("TZ=US/Pacific");//	
	time_t now;
	struct tm *sp;
	(void) time( &now );
	printf("%s", ctime( &now ) );
	sp = localtime(&now);
	printf("%d/%d/%02d %d:%02d %s\n",
	sp->tm_mon + 1, sp->tm_mday,
	sp->tm_year%100, sp->tm_hour, //??????????????/
	sp->tm_min, tzname[sp->tm_isdst]);
	exit(0);
}

/*
* Системные часы Unix используют Всемирное координированное время (UTC), которое приблизительно соответствует среднему времени по Гринвичу (GMT). 
* Именно такое время возвращает системный вызов time(2) и такое же время используется во многих временных штампах, например во временах создания и модификации файлов.
* Значение, возвращаемое time(2) определено как число секунд с 0 часов 0 минут 0 секунд по UTC 1 января 1970 года.  
* Разумеется, если часы вашего компьютера идут неточно, time(2) может возвращать ошибочные значения.
*/
/*
* Допустимые значения TZ описаны на странице руководства environ(5). Наиболее употребительный формат значения TZ, 
* применяемый в современных Unix-системах — это имя файла в каталоге /usr/share/lib/zoneinfo/ . 
* В поставку Solaris входит обширная база описаний часовых поясов, 
* основанная на так называемой IANA Time Zone Database,
* содержащая не только смещения от UTC и правила перевода летнего и зимнего времени для практически всех административных часовых поясов в мире, 
* но и исторические изменения указанных сведений.  
* Так, для всех часовых поясов на территории бывшей Российской Империи указано,
* что до 1919 года время на этих территориях имело смещение от UTC, не кратное часу, 
* потому что отсчитывалось не от Гринвича, а от меридиана Пулковской обсерватории.  
* При изменениях часовых поясов, база данных обновляется.  
* Пользователи Solaris получают эти обновления вместе с остальными обновлениями операционной системы.
*/