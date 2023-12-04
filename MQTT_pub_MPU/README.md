## Medición del MPU6050

En la carpeta ```src``` se encuentran dos códigos:

• ```calibration.cpp``` sirve para ver los valores máximos y mínimos que se pueden medir en cada eje, aparte muestra mucha data de configuración del MPU, que, en nuetro caso no la tuvimos que modificar.

• ```main.cpp``` es el código que efectivamente lee los datos del mpu y los manda por MQTT para que los reciba la esp de la torreta

### Importante

En la carpeta ```lib``` están todas las librerías que se usaron para este código, algunas ya vienen con la instalación del ArduinoIDE, pero las dejamos en caso de que sean necesarias.