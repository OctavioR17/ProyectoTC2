<?php
$usuario = "root";
$contrasena = "embebidos";
$servidor = "localhost";
$database = "accesorfid";

//CREAMOS LA CONEXIÓN CON EL SERVIDOR QUE SE ALMACENARÁ EN $conexion
$conexion = mysqli_connect($servidor, $usuario, $contrasena, $database) or die("No se ha podido conectar con el servidor");


// Verificar si se recibió el dato 'tarjeta'
if (isset($_POST['tarjeta'])) {
    $tarjeta = $_POST['tarjeta'];
} else {
    die("Error: No se recibió el valor de la tarjeta RFID.");
}
//$tarjeta=$_POST["tarjeta"];
//$tarjeta=1151172513;

$querypersona = "SELECT id_persona FROM persona WHERE id_RFID = '$tarjeta'";

$datoid=mysqli_query($conexion, $querypersona);
if (!$datoid) {
    die("Error en la consulta: " . mysqli_error($conexion));
}

// Verificar si se encontraron resultados
if (mysqli_num_rows($datoid) > 0) {
    // Obtener los resultados en un array asociativo
    while ($fila = mysqli_fetch_assoc($datoid)) {
        // Imprimir el valor de 'id_persona'
        $idpersona = $fila['id_persona']; 
        echo "ID Persona: " . $fila['id_persona'] . "<br>";
    }
} else {
    echo "No se encontró ninguna persona con ese ID RFID.";
}
// Cerrar la conexión
//$idpersona
$insertar_acceso="INSERT INTO control_acceso (id_Acceso, Persona_id) VALUES (NULL, '$idpersona')";
if (mysqli_query($conexion, $insertar_acceso)) {
    echo "Datos ingresados correctamente.";
} else {
    echo "Error al insertar datos: " . mysqli_error($conexion);
}
mysqli_close($conexion);

//echo "Datos ingresados";
?>