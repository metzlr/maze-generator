<?php
ini_set('memory_limit', '256M');
if ($_SERVER["REQUEST_METHOD"] == "GET") {
    $data = htmlspecialchars($_GET);
    $blocky = isset($_GET['blocky']) ? $_GET['blocky'] : "false";
    $blocky_arg = "";
    if ($blocky == "true") {
        $blocky_arg = "--blocky";
    }
    $width = isset($_GET['width']) ? intval($_GET['width']) : 10;
    if ($width < 4) {
        $width = 4;
    } else if ($width > 150) {
        $width = 150;
    }
    $height = isset($_GET['height']) ? intval($_GET['height']) : 10;
    if ($height < 4) {
        $height = 4;
    } else if ($height > 150) {
        $height = 150;
    }
    $resolution = isset($_GET['resolution']) ? intval($_GET['resolution']) : 2;
    if ($resolution < 1) {
        $resolution = 1;
    } else if ($resolution > 3) {
        $resolution = 3;
    }
    $radius = isset($_GET['radius']) ? floatval($_GET['radius']) : 0;
    if ($radius < 0) {
        $radius = 0;
    } else if ($radius > 10) {
        $radius = 10;
    }
    $startType = isset($_GET['startType']) ?  $_GET['startType'] : "edge";
    $centerX;
    $centerY;
    $startString = "";
    if ($startType == "center") {
        $centerX = isset($_GET['centerX']) ? intval($_GET['centerX']) : 0;
        $centerY = isset($_GET['centerY']) ? intval($_GET['centerY']) : 0;
        if ($centerX < 0) {
            $centerX = 0;
        } else if ($centerX > 20) {
            $centerX = 20;
        }
        if ($centerY < 0) {
            $centerY = 0;
        } else if ($centerY > 20) {
            $centerY = 20;
        }
        $startString = " -center ". $centerX ." ". $centerY;
    }
    //error_log("../maze-generator-scripts/maze_generator -width ". $width ." -height ". $height . " -detail ". $resolution ." -radius ". $radius . $startString . " --2d ". $blocky_arg ." --solution", 0);
    $mazeData = shell_exec("../maze-generator-scripts/maze_generator -width ". $width ." -height ". $height . " -detail ". $resolution ." -radius ". $radius . $startString . " --2d ". $blocky_arg ." --solution");
    http_response_code(200);
    echo json_encode($mazeData);
}
exit();
?>
