#include "include/tlib.h"

QString tlib::conformCostString(double cost)
{
	QString s = QString::number(cost, 'f', 2);
	int i = s.indexOf('.');
	if (i == -1)
		i = s.size();
	for (i-=3; i > 0; i-=3)
		s.insert(i, ' ');

	return s.replace('.', ',');
}
