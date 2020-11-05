package main

import (
	"net/http"

	"github.com/labstack/echo"
)

func main() {
	e := echo.New()
	e.GET("/", func(c echo.Context) error {
		return c.String(http.StatusOK, "Bienvenido a la Api")
	})
	go e.POST("/guardarData", guardarData)
	go e.GET("/obtenerData", obtenerData)

	e.Start(":8080")
}
