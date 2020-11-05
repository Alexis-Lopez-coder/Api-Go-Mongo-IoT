package main

import (
	"context"
	"fmt"
	"log"
	"net/http"
	"time"

	"github.com/labstack/echo"
	"go.mongodb.org/mongo-driver/bson"
)

type Apagador struct {
	Estado string    `json:"estado"`
	UltAct time.Time `json:"ult_act"`
}

var apagadorCollection = db().Database("PruebaApi").Collection("PruebaApi")
var ctx = context.Background()

func guardarData(c echo.Context) error {
	d := new(Apagador)
	if err := c.Bind(d); err != nil {
		return err
	}
	d.UltAct = time.Now()

	insertResult, err := apagadorCollection.InsertOne(ctx, d)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Print(c.JSON(http.StatusCreated, insertResult.InsertedID))
	return c.JSON(http.StatusCreated, insertResult.InsertedID)

}

func obtenerData(c echo.Context) error {
	cursor, err := apagadorCollection.Find(ctx, bson.M{})
	if err != nil {
		fmt.Println(err)
	}

	apagadores := []Apagador{}
	for cursor.Next(ctx) {
		var p Apagador
		if err := cursor.Decode(&p); err != nil {
			log.Fatal("cursor. Decode ERROR:", err)
		}
		apagadores = append(apagadores, p)
	}

	return c.JSON(http.StatusOK, apagadores)
}
