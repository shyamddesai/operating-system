The following document is based on Professor Christophe Dubach's README for the COMP520-W22 repository.

# Description of coursework (likely to be updated)! #
The description of the coursework may be updated from time to time to add clarifications or fix mistakes.
We encourage you to regularly check this repository and/or MyCourses for changes.

# Software requirements
### Recommended: using CS machines (Mimi)
If you use CS department lab machines, all the software required for development should be already installed. The CS machines 
grant you access to a Linux home folder where you can keep your repository and do your work. This home folder is shared 
amongst all CS machines when you log in using your CS account.

To log in (SSH) into Mimi use the following command:
```
ssh <your_cs_username>@mimi.cs.mcgill.ca
```

It will ask for your CS account password. You will be dropped into a terminal in your home directory (`~`).

We recommend you clone your team's fork of the code into your home directory:
```
git clone https://gitlab.cs.mcgill.ca/<your_cs_username>/comp310-winter23.git
```

After that, you can use Visual Studio Code SSH feature to modify the files in Mimi from your own machine. Follow these instructions:
https://code.visualstudio.com/docs/remote/ssh

### Using your own machine
If you wish to develop using your own machine, you will need to ensure that you have the following software installed:

1. Git (use the following tutorial https://www.atlassian.com/git/tutorials/install-git)
2. GNU C Compiler (>=9.4.0). On Mac, `gcc` calls `clang`, which should be compatible, but **you may face issues**. On Windows, you can use [cygwin](https://www.cygwin.com/) to install the GNU tools.
3. GNU Make (>=4.2.1). Same as above.

# Tests #
Your code will be tested against the scenarios in the `testcases` directory. For example, the `echo.txt` test passes 
batch commands to your shell, the expected output is found on `echo_result.txt`.

## Autograder
The autograder compiles the code in the `main` branch of your repository and then runs it against all the tests cases. 
It will send an email report to the users that have a public email address in their GitLab profile.

This process happens every day. The autograder uses the version of the code found on your repo at 00:00am Montreal Time.

### :exclamation: Important
Make sure you DO NOT change the `Makefile` nor the directory structure! If you do, the autograder may not be able to compile your code and will probably fail you.

# Setup #

## 1. Setting a public email
At least one member of each team needs to set a public email on their GitLab profile in order to receive the autograder reports.

## 2. Forking the repository
We are going to be using the Git revision control system during the course.
If you use your own machine then make sure to install Git.

Only one person per team needs to follow these steps. The team as a whole will develop their code on the forked repository. 
In order to fork this repository:

1. Click the `Fork` button in the upper right-hand side
2. In the fork options, use your user namespace (same as your username). Ensure the repository visibility is **Private**.
3. In your fork, in the left hand side menu, go to Project information > Members
   1. Grant access to your teammates with the **Owner** permission
   2. Grant access to the teaching staff with the **Reporter** permission:
      * Oana BALMAU (username: balmau)
      * Sebastian ROLON (username: jrolon)

## 3. Cloning your team's fork
Everyone on the team should follow these instructions. You will have to clone the forked repository to your local machine. 
You can clone the repository using either HTTPS or SSH.
Using SSH is more secure, but requires
[uploading a private key to GitLab](https://docs.gitlab.com/ee/ssh/). HTTPS is less secure but simpler as it only
requires you to enter your CS account username and password. If in doubt, HTTPS is sufficient.

In order to clone the repository via SSH you should ensure that you've uploaded a private key to GitLab, launch a terminal, and type:

```
$ git clone git@gitlab.cs.mcgill.ca:XXXXXXXX/comp310-winter23.git
```

Where XXXXXXXX is the CS gitlab account id of the teammate who created the fork.

In order to clone the repository via HTTPS you should launch a terminal and type:

```
$ git clone https://gitlab.cs.mcgill.ca/XXXXXXXX/comp310-winter23.git
```

where XXXXXXX is the CS gitlab account id of the fork creator (as above), and you should be prompted to type in your CS gitlab account id and password.

## 4. Keeping your fork updated
This repository will be updated constantly, and your fork does not update automatically. You have to check this repo periodically and
update your fork.

This has to be done manually. To set this up, run:

```
git remote add upstream git@gitlab.cs.mcgill.ca:balmau/comp310-winter23.git
git checkout main
git pull upstream main # update local from original
git push origin main # update fork in gitlab
```

Local Git repos can have multiple remotes. The default remote is `origin` which refers to your fork in GitLab. The commands above 
add the `upstream` remote, which refers to the repo you forked.

You can pull from `upstream` directly to update your local copy, and then you can push to your fork.

Next time you do this, you only need to run the last 3 commands. Your local Git copy will remember the `upstream` remote.
