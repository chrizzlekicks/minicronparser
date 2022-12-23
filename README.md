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

Let's imagine the input file looks like this:
```
30 1 /bin/run_me_daily
45 * /bin/run_me_hourly
...
```

The program will print the scheduled cron jobs in a proper format and additionally determines whether the job will be fired today
or tomorrow.
```
01:30 tomorrow - /bin/run_me_daily
16:45 today - /bin/run_me_hourly
...
```

The parsing function is smart enough to identify outliers and acts accordingly. For example, if you try to pass the current time like
`24:60`, the program will automatically convert into the right format like so: `01:00`. Why? Because 60 minutes add up to a full hour,
therefore the hour gets +1 as the following hour would be reached, and 24 hours convert to 0 and hence, restart the clock. Feel free to 
try it with times like `23:60`, `24:00`, but also see what happens when you try to enter numbers above 24 for hours and 60 for minutes.

In order to get rid of the executable, simply use the following command in the root of the project directory
```
make clean
```

In case of questions, feel free to get in touch: chris.schimetschka@gmail.com 
