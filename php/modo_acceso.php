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
$sql = "select id_Acceso, Persona_id, Acceso from control_Acceso;";
$resultado = $conexion->query($sql);
?>

<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Modo de Acceso</title>
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
        .access-panel {
            background-color: white;
            padding: 20px;
            border-radius: 10px;
            display: inline-block;
            box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1);
        }
        .access-result {
            font-size: 24px;
            margin: 20px 0;
            color: #007bff;
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
        <div class="access-panel">
            <h2>Modo de Acceso</h2>
            <div class="access-result">
                <!-- Aquí se mostraría "Acceso Permitido" o "Acceso Denegado" basado en la lógica PHP -->
                <strong>Acceso Permitido</strong>
            </div>
        </div>

        <div class="data-table">
            <h3>Historial de Accesos</h3>
            <table>
                <thead>
                    <tr>
                        <th>ID Acceso</th>
                        <th>ID Persona</th>
                        <th>Fecha y Hora</th>
                    </tr>
                </thead>

                <?php 
                    while($datos=$resultado->fetch_array()){
                        ?>
                    <tr>
                        <td><?php echo $datos["id_Acceso"]?></td>
                        <td><?php echo $datos["Persona_id"]?></td>
                        <td><?php echo $datos["Acceso"]?></td>
                    </tr>
                    <?php
                    }
               
                ?>

            </table>
        </div>
    </div>
</body>
</html>
