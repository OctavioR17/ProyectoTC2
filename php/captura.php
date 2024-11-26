<?php
$usuario = "root";
$contrasena = "embebidos";
$servidor = "localhost";
$database = "accesorfid";

//CREAMOS LA CONEXIÓN CON EL SERVIDOR QUE SE ALMACENARÁ EN $conexion
$conexion = mysqli_connect($servidor, $usuario, $contrasena, $database) or die("No se ha podido conectar con el servidor");

if($conexion->connect_errno)
{
    echo "Error de conexion de la base datos".$conexion->connect_error;
    exit();
}
$sql = "select id_Persona, Nombre, ApellidoPaterno, ApellidoMaterno, id_RFID from persona;";
$resultado = $conexion->query($sql);
?>

<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Captura de Datos</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            text-align: center;
            margin: 0;
            padding: 0;
        }
        .container {
            margin-top: 50px;
        }
        .form {
            background-color: white;
            padding: 20px;
            border-radius: 10px;
            display: inline-block;
            box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1);
        }
        .form input {
            margin: 10px;
            padding: 10px;
            border: 1px solid #ccc;
            border-radius: 5px;
            width: 80%;
        }
        .form button {
            background-color: #007bff;
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin: 10px;
        }
        .form button:hover {
            background-color: #0056b3;
        }
        .data-table {
            margin-top: 20px;
        }
        table {
            width: 80%;
            margin: 20px auto;
            border-collapse: collapse;
            background-color: white;
        }
        th, td {
            padding: 10px;
            border: 1px solid #ccc;
        }
        th {
            background-color: #007bff;
            color: white;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="form">
            <h2>Captura de Datos</h2>
            <form action="captura_procesar.php" method="POST">
                <input type="text" name="nombre" placeholder="Nombre" required><br>
                <input type="text" name="apellidopaterno" placeholder="Apellido Paterno" required><br>
                <input type="text" name="apellidomaterno" placeholder="Apellido Materno" required><br>
                <input type="text" name="id_rfid" placeholder="Identificador RFID" required><br>
                <button type="submit">Registrar</button>
            </form>
        </div>

        <div class="data-table">
            <h3>Datos Registrados</h3>
            <table>
                <thead>
                    <tr>
                        <th>ID</th>
                        <th>Nombre</th>
                        <th>Apellido Paterno</th>
                        <th>Apellido Materno</th>
                        <th>RFID</th>
                    </tr>
                </thead>
               
                    <?php 
                    while($datos=$resultado->fetch_array()){
                        ?>
                    <tr>
                        <td><?php echo $datos["id_Persona"]?></td>
                        <td><?php echo $datos["Nombre"]?></td>
                        <td><?php echo $datos["ApellidoPaterno"]?></td>
                        <td><?php echo $datos["ApellidoMaterno"]?></td>
                        <td><?php echo $datos["id_RFID"]?></td>
                    </tr>
                    <?php
                    }
               
                ?>
            </table>
        </div>
    </div>
</body>
</html>
