package faz_client

import (
	"context"
	"log"
	"os"

	faz "include path to faz proto"


)

type FazClient struct {
	stub_ FazServiceClient
	conn_ *ClientConn
}

// This function acts as a constructor 
// and initializes the member variables 
// of the FazClient struct
func CreateFazClient(address string) FazClient {
	// Set up a connection to the server.
	fazClient := FazClient{}
	fazClient.conn_, err := grpc.Dial(address, grpc.WithInsecure())
	if err != nil {
		log.Fatalf("did not connect: %v", err)
		os.Exit(1)
	}
	// Creating the stub that will call methods
	fazClient.stub_ = faz.NewFazServiceClient(fazClient.conn_)
	// Returning our newly created object
	return fazClient
}

// This function acts as a destructor 
// and does what is needed to destroy the 
// object. NOTE: THIS HAS TO BE CALLED MANUALLY
func (fazClient *FazClient) DestroyFazClient() {
	fazClient.conn_.Close()
}

// This method links to the event method in the server class
// This executes our stored function via our FaaS Faz
// Args: A request and response of protobuf Any type. These
//       represent the input and output from our registered function
// Returns: A boolean indicating success / failure
func (fazClient *FazClient) Event(request *anypb.Any, 
																	response *anypb.Any,
																	eventType int) bool {
	eventRequest := faz.EventRequest{
		EventType: eventType, 
		Payload: request,
	}
	eventResponse, err := fazClient.stub_.Event(context.Background(), &eventRequest)
	if err != nil {
		log.Fatalf("could not perform event: %v", err)
		return false
	} else {
		// Setting the Any payload in eventResponse to
		// our Any response variable
		response = eventResponse.GetPayload()
		return true
	}
}

// This method calls the Faz hook function. This is supposed to
// hook a custom function to our FaaS.
// Args: An event type that corresponds with the function call
//       And a string that represents the function name
// Returns: A boolean indicating success / failure
func (fazClient *FazClient) HookFunction(eventType int,
																				 functionName string) bool {
	hookRequest := faz.HookRequest{
		EventType: eventType,
		EventFunction: functionName,
	}
	hookResponse, err := fazClient.stub_.Hook(context.Background(), &hookRequest)
	if err != nil {
		log.Fatalf("could not hook function: %v", err)
		return false
	} else {
		return true
	}
}

// This method calls the faz unhook function. This is supposed
// to unhook a custom function to our FaaS.
// Args: An event type whose corresponding function we are to
//       unhook.
// Returns: A boolean indicating success / failure
func (fazClient *FazClient) UnhookFunction(eventType int) bool {
	unhookRequest := faz.UnhookRequest{
		EventType: eventType,
	}
	unhookResponse, err := fazClient.stub_.Unhook(context.Background(), &unhookRequest)
	if err != nil {
		log.Fatalf("could not unhook function: %v", err)
		return false
	} else {
		return true
	}
}