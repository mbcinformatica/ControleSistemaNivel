<?php

$servername = "172.28.186.120";
$username = "root";
$password = "password";
$dbname = "iot";
$conn = new mysqli($servername,  $username, $password, $dbname);

if ($conn->connect_error) {
     //Gravar log de erros
     die("Nao foi possivel estabelecer conexao com o Banco de Dados: " . $conn->connect_error);
}

$conn->close();
?>