<?php
$usuario = "root";
$contrasena = "embebidos";
$servidor = "localhost";
$database = "accesorfid";

//CREAMOS LA CONEXIÓN CON EL SERVIDOR QUE SE ALMACENARÁ EN $conexion
$conexion = mysqli_connect($servidor, $usuario, $contrasena, $database) or die("No se ha podido conectar con el servidor");

//$tarjeta=$_POST['tarjeta'];
$nombre=$_POST['nombre'];
$apellidopaterno=$_POST['apellidopaterno'];
$apellidomaterno=$_POST['apellidomaterno'];
$id_rfid=$_POST['id_rfid'];
$insertar_acceso="INSERT INTO persona (nombre, ApellidoPaterno, ApellidoMaterno, id_RFID) VALUES ('$nombre', '$apellidopaterno','$apellidomaterno','$id_rfid')";
if (mysqli_query($conexion, $insertar_acceso)) {
    echo "Datos ingresados correctamente.";
} else {
    echo "Error al insertar datos: " . mysqli_error($conexion);
}
mysqli_close($conexion);
//echo "Datos ingresados";
?>