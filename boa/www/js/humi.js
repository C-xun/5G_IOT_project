function getXMLHttpRequest()
{
    var xmlhttp = null;
    if(window.XMLHttpRequest)
    {
        xmlhttp = new XMLHttpRequest();
    }
    else
    {
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    return xmlhttp;
}

function my_humi()
{
    var xmlhttp = getXMLHttpRequest();

    xmlhttp.onreadystatechange = function () {
        if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
            document.getElementById("humi").value = xmlhttp.responseText;
        }
    }

    url = "/cgi-bin/humi.cgi?";

    xmlhttp.open("GET", url, true);
    xmlhttp.send();
}
setInterval("my_humi()",5500);