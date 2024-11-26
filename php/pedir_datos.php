
<?php
// Datos de conexión
$conexion = mysqli_connect("localhost", "root", "embebidos", "accesorfid");

// Verificar la conexión
if (!$conexion) {
    die("Error al conectar a la base de datos: " . mysqli_connect_error());
}

// Sanitizar el valor de id_rfid para evitar inyecciones SQL
$id_rfid = mysqli_real_escape_string($conexion, $_GET['id_rfid']);

// Consulta SQL
$sql = "SELECT id_rfid FROM persona WHERE id_rfid='$id_rfid'";
$resultado = mysqli_query($conexion, $sql);

// Verificar si se obtuvieron resultados
if (mysqli_num_rows($resultado) > 0) {
    // Obtener el primer resultado
    $fila = mysqli_fetch_assoc($resultado);
    $nombre = $fila['id_rfid'];
    echo "valor=" . $nombre . ";";
} else {
    echo "No se encontró ningún resultado.";
}

// Cerrar la conexión
mysqli_close($conexion);
/*<?php
$conexion = mysql_connect("localhost", "root", "embebidos");
mysql_select_db("accesorfid",$conexion);
$resultado = mysql_query("SELECT id_rfid FROM persona WHERE id_rfid='" . $_GET['id_rfid']. "'", $conexion);
$nombre = mysql_result($resultado, 0);
echo "valor=" . $nombre . ";";
<?php
$usuario = "root";
$contrasena = "embebidos";
$servidor = "localhost";
$database = "accesorfid";
$conexion = mysqli_connect($servidor, $usuario, $contrasena, $database) or die("No se ha podido conectar con el servidor");
$resultado = mysql_query("SELECT id_rfid FROM persona WHERE id_rfid='" . $_GET['id_rfid']. "'", $conexion);
$nombre = mysql_result($resultado, 0);
echo "valor=" . $nombre . ";";
?>
?>*/
?>




