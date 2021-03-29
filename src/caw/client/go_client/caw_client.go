package caw_client

import (
	"fmt"
	"strconv"

	"github.com/golang/protobuf/proto"
	"github.com/golang/protobuf/ptypes"
	"github.com/golang/protobuf/ptypes/any"

	faz "github.com/floan/csci499_floan/src/faz/client/go_client"

	caw "github.com/floan/csci499_floan/protos/go_protos/caw"
)

type CawClient struct {
	functionToEventType_ map[string]int
	client_ *faz.FazClient
}

// This function acts as a constructor 
// and initializes the member variables 
// of the CawClient struct
func CreateCawClient() *CawClient {
	cawClient := &CawClient{}
	cawClient.functionToEventType_ = map[string]int{
		"registeruser": 1,
		"caw": 2,
		"read": 3,
		"follow": 4,
		"profile": 5,
	}
	cawClient.client_ = faz.CreateFazClient("localhost:50000")

	return cawClient
}

func (cawClient *CawClient) RegisterUser(username string) bool {
	registerRequest := caw.RegisteruserRequest{
		Username: username,
	}
	anyRequest, errPacking := ptypes.MarshalAny(proto.Message(&registerRequest))
	if errPacking != nil {
		fmt.Println(errPacking.Error())
		return false
	}
	var anyResponse = new(any.Any)
	registerSuccessBool := cawClient.client_.Event(anyRequest, anyResponse, 
													cawClient.functionToEventType_["registeruser"])
	// No need to unpack the response for Registeruser
	if registerSuccessBool {
		fmt.Println("User registered successfully!")
		return true
	} else {
		errRegistering := fmt.Errorf("There was an error in registering the user")
		fmt.Println(errRegistering.Error())
		return false
	}
} 

func (cawClient *CawClient) PostCaw(username string,
																		text string,
																		parent_id int) bool {
	var cawRequest caw.CawRequest
	// Checking if this is a reply or
	// a brand new caw
	if parent_id != -1 {
		cawRequest = caw.CawRequest{ Username: username, Text: text, 
																 ParentId: []byte(strconv.Itoa(parent_id)) }
	} else {
		cawRequest = caw.CawRequest{ Username: username, Text: text }
	}

	anyRequest, errPacking := ptypes.MarshalAny(proto.Message(&cawRequest))
	if errPacking != nil {
		panic(errPacking)
	}
	var anyResponse = new(any.Any)
	cawSuccessBool := cawClient.client_.Event(anyRequest, anyResponse,
													cawClient.functionToEventType_["caw"])
	if cawSuccessBool {
		var cawResponse = new(caw.CawReply)
		errUnpacking := ptypes.UnmarshalAny(anyResponse, cawResponse)
		if errUnpacking != nil {
			fmt.Println(errUnpacking.Error())
			return false
		} else {
			caw := cawResponse.GetCaw()
			fmt.Println("Your caw was posted successfully!")
			fmt.Println("Here's your caw information")
			fmt.Printf("\t Username: %s \n", caw.Username)
			fmt.Printf("\t Text: %s \n", caw.Text)
			fmt.Printf("\t Id: %s \n", string(caw.Id))
			fmt.Printf("\t Parent_Id: %s \n", string(caw.ParentId))
			fmt.Printf("\t Time created: %d \n", caw.Timestamp.Seconds)
			return true
		}
	} else {
		return false
	}
}

func (cawClient *CawClient) FollowUser(username string, 
																			 to_follow string) bool {
	followRequest := caw.FollowRequest{
		Username: username,
		ToFollow: to_follow,
	}
	anyRequest, errPacking := ptypes.MarshalAny(proto.Message(&followRequest))
	if errPacking != nil {
		fmt.Println(errPacking.Error())
		return false
	}
	var anyResponse = new(any.Any)
	followSuccessBool := cawClient.client_.Event(anyRequest, anyResponse,
												cawClient.functionToEventType_["follow"])
	if followSuccessBool {
		fmt.Println("User followed successfully!")
		return true
	} else {
		errFollowing := fmt.Errorf("There was an error in following the user")
		fmt.Println(errFollowing.Error())
		return false
	}
}

	//TODO: ADD ERROR MESSAGES IN THE ELSE SPACE 

func (cawClient *CawClient) ReadCaw(caw_id int) bool {
	readRequest := caw.ReadRequest{
		CawId: []byte(strconv.Itoa(caw_id)),
	}
	anyRequest, errPacking := ptypes.MarshalAny(proto.Message(&readRequest))
	if errPacking != nil {
		fmt.Println(errPacking.Error())
		return false
	}
	var anyResponse = new(any.Any)
	readSuccessBool := cawClient.client_.Event(anyRequest, anyResponse,
											cawClient.functionToEventType_["read"])
	if readSuccessBool {
		var readResponse = new(caw.ReadReply)
		errUnpacking := ptypes.UnmarshalAny(anyResponse, readResponse)
		if errUnpacking != nil {
			fmt.Println(errUnpacking.Error())
			return false
		} else {
			caws := readResponse.GetCaws()
			fmt.Println("Here's your thread information: ")
			for _, caw := range caws {
				fmt.Printf("\t Username: %s \n", caw.Username)
				fmt.Printf("\t Text: %s \n", caw.Text)
				fmt.Printf("\t Id: %s \n", string(caw.Id))
				fmt.Printf("\t Parent_Id: %s \n", string(caw.ParentId))
				fmt.Printf("\t Time created: %d \n", caw.Timestamp.Seconds)
				fmt.Printf("==============================\n")
			}
			return true
		}
	} else {
		return false
	}
}

func (cawClient *CawClient) GetProfile(username string) bool {
	profileRequest := caw.ProfileRequest{
		Username: username,
	}
	anyRequest, errPacking := ptypes.MarshalAny(proto.Message(&profileRequest))
	if errPacking != nil {
		fmt.Println(errPacking.Error())
		return false
	}
	var anyResponse = new(any.Any)
	profileSuccessBool := cawClient.client_.Event(anyRequest, anyResponse,
											cawClient.functionToEventType_["profile"])
	if profileSuccessBool {
		var profileResponse = new(caw.ProfileReply)
		errUnpacking := ptypes.UnmarshalAny(anyResponse, profileResponse)
		if errUnpacking != nil {
			fmt.Println(errUnpacking.Error())
			return false
		} else {
			fmt.Println("We have retrieved the profile!")
			fmt.Println("Here's your information: ")
			fmt.Println("Here are user's followers: ")
			followers := profileResponse.Followers
			for _, follower := range followers {
				fmt.Printf("\t %s", follower)
			}
			fmt.Println("Here are the user's following: ")
			following := profileResponse.Following
			for _, user := range following {
				fmt.Printf("\t %s", user)
			}
			return true;
		}
	} else {
		return false
	}
}

func (cawClient *CawClient) HookFunction(functionName string) bool {
	eventType := cawClient.functionToEventType_[functionName]
	hookSuccessBool := cawClient.client_.HookFunction(eventType, functionName)
	if hookSuccessBool {
		fmt.Println("Your function was successfully hooked.")
		return true
	} else {
		//TODO: MAKE THIS IN ERROR FORMAT
		fmt.Println("Something went wrong when hooking your function.")
		return false
	}
}

func (cawClient *CawClient) UnhookFunction(functionName string) bool {
	eventType := cawClient.functionToEventType_[functionName]
	unhookSuccessBool := cawClient.client_.UnhookFunction(eventType)
	if unhookSuccessBool {
		fmt.Println("Your function was successfully unhooked.")
		return true
	} else {
		//TODO: MAKE THIS IN ERROR FORMAT
		fmt.Println("Something went wrong when unhooking your function.")
		return false
	}
}

func (cawClient *CawClient) HookAll() bool {
	for functionName, eventType := range cawClient.functionToEventType_ {
		hookSuccessBool := cawClient.client_.HookFunction(eventType, functionName)
		if !hookSuccessBool {
			//TODO: MAKE ERROR FORMAT
			fmt.Println("There was a problem hooking all functions.")
			return false
		} //Else do nothing, keep hooking
	}
	fmt.Println("Successfully hooked all functions.")
	return true
}

func (cawClient *CawClient) UnhookAll() bool {
	for _, eventType := range cawClient.functionToEventType_ {
		unhookSuccessBool := cawClient.client_.UnhookFunction(eventType)
		if !unhookSuccessBool {
			//TODO: MAKE ERROR FORMAT
			fmt.Println("There was a problem unhooking all functions.")
			return false
		} //Else do nothing, keep hooking
	}
	fmt.Println("Successfully unhooked all functions.")
	return true
}
