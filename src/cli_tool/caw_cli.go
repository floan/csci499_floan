package main

import (
	"flag"
	"fmt"
	"os"

	caw "github.com/floan/csci499_floan/src/caw/client/go_client"
)

// Defining command line flags
var (
	registeruserPtr = flag.String("registeruser", "", "Registers the given username")
	userPtr         = flag.String("user", "", "Logs in as the given username")
	cawPtr          = flag.String("caw", "", "Creates a new Caw with the given text")
	replyPtr        = flag.Int("reply", -1,
														 "Indicates that the new caw is a reply to the given id")
	followPtr  = flag.String("follow", "", "Starts following the given username")
	readPtr    = flag.Int("read", -1, "Reads the caw thread starting at the given id")
	profilePtr = flag.Bool("profile", false,
												 "Gets the user’s profile of following and followers")
	hookPtr      = flag.String("hook", "", "Hooks the given function")
	unhookPtr    = flag.String("unhook", "", "Unhooks the given function")
	hookallPtr   = flag.Bool("hookall", false, "Hooks all stored functions")
	unhookallPtr = flag.Bool("unhookall", false, "Unhooks all stored functions")
)

func main() {
	// Checking and handling arguments
	if len(os.Args) < 2 {
		fmt.Println("You have not used any command line arguments. Please try again.")
		flag.PrintDefaults()
		os.Exit(1)
	}

	flag.Parse()

	// Ensuring a username is always specified
	// With exception of registeruser
	if *registeruserPtr == "" && *userPtr == "" && *hookPtr == "" &&
		*unhookPtr == "" && *unhookallPtr == false &&
		*hookallPtr == false {
		err := fmt.Errorf("You cannot perform any action other than " +
											"hooking/unhooking without specifying a username. \n" +
											"Please consider using the --user <username> flag.")
		fmt.Println(err.Error())
		os.Exit(1)
	}

	if *hookallPtr && *unhookallPtr {
		err := fmt.Errorf("You cannot hook all and unhook all simultaneously.")
		fmt.Println(err.Error())
		os.Exit(1)
	}

	if *replyPtr != -1 && *cawPtr == "" {
		err := fmt.Errorf("You cannot reply without specifying a caw. \n" +
											"Please consider using the --caw <caw text> flag.")
		fmt.Println(err.Error())
		os.Exit(1)
	}

	// Error checking done, now
	// we can handle arguments

	// Creating client instance
	client := caw.CreateCawClient()

	if *registeruserPtr != "" {
		client.RegisterUser(*registeruserPtr)
	} else if *replyPtr != -1 {
		client.PostCaw(*userPtr, *cawPtr, *replyPtr)
	} else if *cawPtr != "" {
		// Since we have already checked for reply
		// We know this would be a caw and not a reply
		// According to specs, we specify a new caw using -1
		client.PostCaw(*userPtr, *cawPtr, -1)
	} else if *followPtr != "" {
		client.FollowUser(*userPtr, *followPtr)
	} else if *readPtr != -1 {
		client.ReadCaw(*readPtr)
	} else if *profilePtr {
		client.GetProfile(*userPtr)
	} else if *hookPtr != "" {
		client.HookFunction(*hookPtr)
	} else if *unhookPtr != "" {
		client.UnhookFunction(*unhookPtr)
	} else if *hookallPtr {
		client.HookAll()
	} else if *unhookallPtr {
		client.UnhookAll()
	}
}
