# ALog

An activity logger that logs activity datetime, info, and duration.

## Description

ALog stands for Activity Logger.

The program can be use to track the time spent on an activity. Each activity session log can also include a comment or description of the session such as what was done during that session.

## Build

Run `make` to build the alog program executable. The executable is called `alog`

You can make `alog` available as a user command by adding it to your bin folder. Alternatively, you could copy the executable to your directory of choice, then add the directory to your PATH environment variable.

### Install command auto-complete (bash)

ALog's commands and options auto-complete could be installed by running the command `sudo make install-autocomplete`. Note that `sudo` is required because the make command is copying the bash completion script into `/etc/bash_completion.d/` which is a root-owned directory.

After the completion script has been copied, you'll need to run the command `. /etc/bash_completion.d/alog` to source the completion script.

To see the program's auto-complete in action, type `alog`, followed by a space, then press tab to see the auto-complete suggestions.

## Test

Run `make test-runner` to build the test program executable, then execute the test program: `./test-runner`

Passed test cases are written to STDOUT, and failed test cases are written to STDERR.

## Usage

### Starting an activity session

```
alog start activity_name
```

Start an activity session.

This will start a session timer. The session could be stopped with the `stop` command. The timer elapsed time could be viewed with the `info` command.

### Stopping an activity session

```
alog stop activity_name [-m message]
```

Stop an active in-progress activity session and log it.

Providing the `-m` option alog with a message (e.g. `alog stop task_1 -m "Define requirements."`) will log the activity session with the provided message. However, if the option and message isn't provided, then the text editor `Nano` will open up to allow user to input the activity session message/description.

### Viewing activity info

```
alog info activity_name [-a] [-r] [-s]
```

View the info of an activity.

By default it display both the recorded and active (in-progress) activities (correspond with the `-ar` option). Providing the `-a` option will display the info (session start datetime and elapsed time) of the active in-progress activity session. Providing the `-r` option will display the logs of the recorded activity. Providing the `-s` option will silence the log messages of the recorded activity.

### Listing activities

```
alog list [-a] [-r]
```

List all available activities.

By default it list only the recorded activities (correspond with the `-r` option). Providing the `-a` option will list the active in-progress activity sessions.

### Cancelling an activity session

```
alog cancel activity_name [-f]
```

Cancel an activity session. 

By default, the program will prompt for user confirmation input before doing the cancellation. However, if the `-f` option is provided, then the prompt is disabled and the activity session is cancelled immediately.

### Deleting an activity

```
alog delete activity_name [-f]
```

Delete a recorded activity log file. 

By default, the program will prompt for user confirmation input before doing the deletion. However, if the `-f` option is provided, then the prompt is disabled and the log file is deleted immediately.

### Viewing documentation

```
alog help
```
Display alog documentation.

## Usage Example
```
$ alog start actv_test
Recording activity: "actv_test".

$ alog list -ar
[RECORDED]
task_1
task_2
[ACTIVE]
actv_test

$ alog info actv_test 
[RECORDED]
[ACTIVE]
Tue Jun  2 18:43:06 2026
Elapsed Time: 0 hours 12 minutes 6 seconds

$ alog stop actv_test -m "Hello, world!"
Activity recorded.

$ alog list -ar
[RECORDED]
task_1
task_2
actv_test
[ACTIVE]

$ alog info actv_test 
[RECORDED]
Tue Jun  2 18:43:06 2026
Elapsed Time: 0 hours 13 minutes 5 seconds

Hello, world!

==========
[ACTIVE]

$ alog delete actv_test 
Are you sure you want to delete: "actv_test"? (y|n):    y
Deleted activity: "actv_test".

$ alog list -ar
[RECORDED]
task_1
task_2
[ACTIVE]
```

## Log file location

All activity logs are stored in `$HOME/.alog/`

On-going (or in-progress) activity sessions are stored in `$HOME/.alog/.tmp/`

## Feature Requests and Issues

To make a feature request or to raise an issue, please create a new issue at [https://github.com/thanakritlee/alog/issues/new](https://github.com/thanakritlee/alog/issues/new)
