package main

import (
	"context"
	"fmt"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
	"log"
)

func db() *mongo.Client {
	// Connect to //MongoDB
	var clientOption = options.Client().ApplyURI("mongodb://localhost:27017")

	client, err:= mongo.Connect(context.TODO(), clientOption)
	if err != nil {
		log.Fatalf("mongo.Connect() ERROR: %v", err)
	}

	//check the connection
	err = client.Ping(context.TODO(), nil)

	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Conexion a MongoDB!")
	return client
}
