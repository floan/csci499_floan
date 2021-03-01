# Welcome To Caw & Faz! 

## About the project

### Faz

Faz is a Function as a Service (FaaS) platform. It performs basic hooking and unhooking of functions and, of course, lets you run your custom function! You can read more about FaaS [here](https://en.wikipedia.org/wiki/Function_as_a_service).

### Caw 

Caw is a social networking platform built on top of Faz. Caw has basic functionality that resembles that of Twitter! 

## Getting set up! 

### Setting up the VM

For best performance, you would want to use a Virtual Machine to prevent any environment discrepencies. 

If you are new to Vagrant, you can read more about it and download it [here](https://www.vagrantup.com/intro).

If you are a Vagrant pro, we recommend the following steps. 

Type in `vagrant init ubuntu/focal64` or change your vagrantfile to `config.vm.box = "ubuntu/focal64"` to match the VM used to built this project. Generally, any other modern Ubuntu version would work too! 

To start your vagrant VM type in the following two commands: 
```
1. vagrant up 
2. vagrant ssh
```

You would also want to make sure your image is up to date by running `sudo apt-get update && sudo apt-get upgrade`

### Setting up Faz & Caw! 

Follow the steps to set up our FaaS and Social Network service! 
```
1. sudo apt-get install build-essential
2. sudo apt-get install cmake
3. git clone git@github.com:floan/csci499_floan.git
4. cd csci499_floan
5. mkdir build
6. cd build
7. cmake .. (This step will take some time, don't panic!)
8. make (This will also take some time!)
```


## Testing 

Now that you've set everything up. Lets make sure it all works! To run the Caw Tests run the command `./caw_test` and to run the key value store tests run the command `./kv_store_test`. 

Everything should be green and passing! Both these commands are to be run inside the build folder. 

## Starting up the application 

In order to fully use the application, you will have to start up some servers. Please open 3 terminals and navigate to the build folder. Enter these two commands on DIFFERENT terminals. 

```
1. ./faz
2. ./kv_store
```

The third terminal will be yours to use with Caw! Read on to find out how! 

## Using Caw! 

Here is a list of all of Caws features!

1. `registeruser` to register a user!
2. `user` to specify whose account is using the app! 
3. `caw` to post a new caw!
4. `reply` to reply to an existing caw!
5. `follow` to follow a fellow caw user!
6. `read` to read a thread of caws! 
7. `profile` to get a list of the users followers and following! 


Caw is designed to allow the user to interract with it through the command line. All commands should be run within the build folder. Please read on to learn more about the commands and how to use them! 

1. Initially all functions will need to be hooked to our Faz service. In order to do this, run the command `./caw --hookall`. You can also unhook all functions by running `./caw --unhookall`. If you want more flexibility, you can also use `./caw --hook <function name>` and `./caw --unhook <function name>`. Once all functions are hooked, you can start using them! 

2. In order to register a user, you must specify the username with our `--registeruser` flag. Here is an example: `./caw --registeruser "fayez"`

3. In order to post a caw, you must specify the user posting, and the text to post! You can specify the text to post using our `--caw` flag. Here is an example: `./caw --user "fayez" --caw "Hello World!"` Please remember that only registered users can post caws! 

4. In order to reply to a caw, you must specify the user replying, the caw to reply to and the text with which the user wants to reply! You can specify the caw id to reply to using our `--reply` flag. Here is an example: `./caw --user "fayez" --reply 1 --caw "Hi Fayez! From the world."`

5. In order to follow a caw, you must specify your username and the username of who you wish to follow! Here is an example: `./caw --user "fayez" --follow "Adam"`. Please note, you can only follow registered users!

6. In order to read a caw thread, you must specify your username and the thread id which you wish to begin reading from. You can specify the caw id to start reading from using our `--read` flag. Here is an example: `./caw --user "fayez" --read 1`.

7. In order to display a profile, you must specify the username of the profile you wish to see! Here is an example: `./caw --user "fayez" --profile`.

If you are looking for more information, or just feel a little lost, feel free to use the command `./caw --help` for more information! Lastly, if you have any issues with the program, or you want to suggest an upgrade, feel free to create an issue on our github page! 

We hope you enjoy using Caw! Bye! 
