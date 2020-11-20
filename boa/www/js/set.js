function temp_humi_set()
{
    var tempMAX = document.getElementById("tempMax");
    var tempMAX_text = tempMAX.value;
    var tempMin = document.getElementById("tempMin");
    var tempMin_text = tempMin.value;
    var temp = document.getElementById("temp");
    var temp_text = temp.value;
    var humiMAX = document.getElementById("humiMax");
    var humiMAX_text = humiMAX.value;
    var humiMin = document.getElementById("humiMin");
    var humiMin_text = humiMin.value;
    var humi = document.getElementById("humi");
    var humi_text = humi.value;
    
    if((Number(temp_text) > Number(tempMAX_text)) || (Number(temp_text) < Number(tempMin_text))||(Number(humi_text) > Number(humiMAX_text)) || (Number(humi_text) < Number(humiMin_text)))
    {
        document.getElementById("ledWarnText").style.display = "block";
        document.getElementById("warn_Led").style.background = "#FF0004";
        document.getElementById("warn_Led").style.marginLeft = "10px";
        document.getElementById("warn_Led").style.width = "25px";
        document.getElementById("warn_Led").style.height = "25px";
        document.getElementById("warn_Led").style.borderRadius = "50%";
        document.getElementById("warn_Led").style.float = "left";

    }
    else if((Number(temp_text) <= Number(tempMAX_text)) && (Number(temp_text) >= Number(tempMin_text))&&(Number(humi_text) <= Number(humiMAX_text)) && (Number(humi_text) >= Number(humiMin_text)))
    {
        document.getElementById("ledWarnText").style.display = "none";
        document.getElementById("warn_Led").style.background = "#00FE1D";
        document.getElementById("warn_Led").style.marginLeft = "10px";
        document.getElementById("warn_Led").style.width = "25px";
        document.getElementById("warn_Led").style.height = "25px";
        document.getElementById("warn_Led").style.borderRadius = "50%";
        document.getElementById("warn_Led").style.float = "left";
    }
}
setInterval("temp_humi_set()",1000);
