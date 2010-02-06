exec ps -eaf | egrep -i '(filter|server)' | egrep -v '(ksmserver|nepomukserver|egrep|GREP_FOR_SERVER)'
