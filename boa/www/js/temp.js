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

function my_temp()
{
    var xmlhttp = getXMLHttpRequest();

    xmlhttp.onreadystatechange = function () {
        if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
            document.getElementById("temp").value = xmlhttp.responseText;
        }
    }

    url = "/cgi-bin/temp.cgi?";

    xmlhttp.open("GET", url, true);
    xmlhttp.send();
}
setInterval("my_temp()",5500);