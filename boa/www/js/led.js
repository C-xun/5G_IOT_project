function getXMLHttpRequest()
{
    var xmlhttp = null;
    if(window.XMLHttpRequest)//高版本浏览器
    {
        xmlhttp = new XMLHttpRequest();//创建一个对象
    }
    else//低版本浏览器
    {
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    return xmlhttp;
}
function set_led(arg)
{
    
    //创建xmlhttp对象
    var xmlhttp =  getXMLHttpRequest();
    
    //异步：设置回调函数
    xmlhttp.onreadystatechange=function()
    {
        if(xmlhttp.readyState==4 && xmlhttp.status == 200)
        {
            alert(xmlhttp.responseText);
        }
    }

    //open创建一个请求：（重要）GET方法 有？后面加data
    var url="/cgi-bin/led/led.cgi?";
    url+=arg;

    xmlhttp.open("GET", url, true);

    //send发送请求
    xmlhttp.send();
}