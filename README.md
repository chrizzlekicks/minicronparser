# Mini Cron Parser

Created a simplified cron parser, who takes in an txt file with the scheduled cron jobs as first parameter and the current time as
second parameter. The program is entirely written in C and comes with a Makefile to compile it into an executable.

### How to run

Clone the repository and jump into the root directory of the project. Compile the program by using the following command. This will
create a `/bin` folder where the executable is located.
```
make
```

In order to execute the program, you can use the following command with an example time. Do not forget to import your config input!
```
cd bin && ./main input.txt 16:10
```

The program will print the scheduled cron jobs in a proper format and additionally determines whether the job will be fired today
or tomorrow.
```
01:30 tomorrow - /bin/run_me_daily
16:45 today - /bin/run_me_hourly
...
```

In case of questions, feel free to get in touch: chris.schimetschka@gmail.com 
