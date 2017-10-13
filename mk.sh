#!/bin/bash
make clean
#/usr/local/php/bin/phpize
#./configure --with-php-config=/usr/local/php/bin/php-config
make && make install
cp .libs/dbfiller.o /usr/local/php/lib/php/extensions/no-debug-non-zts-20100525/
