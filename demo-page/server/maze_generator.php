<?php
if ($_SERVER["REQUEST_METHOD"] == "GET") {
    $data = htmlspecialchars($_GET);
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
    $mazeData = shell_exec("./scripts/maze_generator_old -w ". $width ." -h ". $height ." -o");
    http_response_code(200);
    echo json_encode($mazeData);
}
exit();
?>
