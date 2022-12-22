The following document is based on Professor Christophe Dubach's README for the COMP520-W22 repository.

# Description of coursework (likely to be updated)! #
The description of the coursework may be updated from time to time to add clarifications or fix mistakes.
We encourage you to regularly check this repository and/or MyCourses for changes.

# Software requirements #
If you use CS department lab machines, all the software required for development should be already installed.
If you wish to develop using your own machine, you will need to ensure that you have the following software installed:

1. Git (use the following tutorial https://www.atlassian.com/git/tutorials/install-git)
2. GNU C Compiler (>=9.4.0). On Mac, `gcc` calls `clang`, which should be compatible, but **you may face issues**. On Windows (not recommended), you can use [cygwin](https://www.cygwin.com/) to install the GNU tools.
3. GNU Make (>=4.2.1). Same as above.

# Tests #
Your code will be tested against the scenarios in the `testcases` directory. For example, the `echo.txt` test passes 
batch commands to your shell, the expected output is found on `echo_result.txt`.

## Autograder
The autograder compiles the code in the `main` branch of your repository every morning Eastern Time, and then runs it against all the tests cases. 
It will send an email report to the users that have a public email address in their GitLab profile.

# Setup #
### GitLab
We will rely on gitlab and it is mandatory to use it for this coursework.
GitLab is an online repository that can be used with the git control revision system.
The CS department runs a GitLab hosting service, and all students should be able to access it with their CS account.

Important: do not share your code and repository with anyone outside your group and keep your source code secret.
If we identify that two groups have identical portion of code, both will be considered to have cheated.

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
