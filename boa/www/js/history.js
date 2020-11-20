/* ************************************************
			此JS函数即为创建XML对象
		   并兼容IE，firefox等浏览器
************************************************ */
function createXHR()
{
	var xmlhttp = null;
	if(window.XMLHttpRequest){
		xmlhttp = new XMLHttpRequest();
	}else if(window.ActiveXObject){
		xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
	}else{
		xmlhttp = new ActiveXObject('Msxml2.XMLHTTP');
	}		
	if(xmlhttp){
		//alert('create xmlhttp success!');
		
	}else{
		//alert('create xmlhttp error!');
	}
	return xmlhttp;
}

/* ************************************************
			得到cgi处理结果
		  并根据服务器状态得到cgi处理结果
************************************************ */

function get_cgi_result(urlcmd, type)
{
	var xmlhttp = createXHR();//cgi js 通信方式
	if(xmlhttp)
	{
		//建立连接请求
		xmlhttp.open("GET",urlcmd,false);//false:同步;true:异步
		xmlhttp.setRequestHeader("If-Modified-Since", "0");//清除缓存
		xmlhttp.send(); //向服务器发送请求，先open()，才能再调用send();
		//alert("status===" +xmlhttp.status);
		if(200 == xmlhttp.status)//服务器的HTTP状态码200对应OK，
		{
			//alert("status===" +xmlhttp.status);
			if(4 == xmlhttp.readyState) //readyState，加载状态，4 代表完成
			{
				//alert("readyState===" +xmlhttp.readyState);
				if(0 == type)
				{
					//alert("responseText===" +xmlhttp.responseText);
					return xmlhttp.responseText;
				}
				else if(1 == type)
				{
					//alert("responseXML===" +xmlhttp.responseXML);
					return xmlhttp.responseXML;
					//return xmlhttp.responseText;(测试用)
				}
			}
		}
	}
	else
	{
		alert('please check the Bowser!');
	}
}
/* ************************************************
			显示查询到的表
************************************************ */
function deal_query_result(data_tmp)
{
	//clear
	var txt="<table id='customers'>";
	txt += "<tr>";
	txt += "<th>温度</th>";
	txt += "<th>湿度</th>";
	txt += "<th>时间</th>";
	txt += "</tr>";
	var x=data_tmp.documentElement.getElementsByTagName("INFO");
	for (i=0;i<x.length;i++)
	{	
	
		if(i % 2 == 0)
		{
			txt=txt + "<tr>";
		}
		else
		{
			txt=txt + "<tr class='alt'>";
		}
		xx=x[i].getElementsByTagName("temp");
		
		try
		{
			txt=txt + "<td>" + xx[0].firstChild.nodeValue + "</td>";
		}
		catch (er)
		{
			txt=txt + "<td> </td>";
		}
		xx=x[i].getElementsByTagName("humi");
		
		try
		{
			txt=txt + "<td>" + xx[0].firstChild.nodeValue + "</td>";
		}
		catch (er)
		{
			txt=txt + "<td> </td>";
		}
		
		xx=x[i].getElementsByTagName("time");
		
		try
		{
			txt=txt + "<td>" + xx[0].firstChild.nodeValue + "</td>";
		}
		catch (er)
		{
			txt=txt + "<td> </td>";
		}
		txt=txt + "</tr>";
	}
	txt=txt + "</table>";
	document.getElementById('copy').innerHTML=txt;
}
/* ************************************************
			js得到cgi处理结果
		  并根据结果打印相应结果
************************************************ */
function js_communicate_with_cgi(urlcmd, type)//0:text;1:xml
{
	var data_tmp = get_cgi_result(urlcmd, type);
	if(0 == type)//text
	{
		alert("text in js_communicate_with_cgi ");
	}
	else//xml
	{
		//alert("xml in js_communicate_with_cgi ");
		deal_query_result(data_tmp);//处理并显示
	}
}
/* ************************************************
				确保查询信息的合法性
************************************************ */
function get_query_info()
{
	var info = "1";
	var query_method = document.getElementById("select_query").value;
	var text = document.getElementById("text_query").value;

	if("" == text)
	{
		alert("输入信息不能为空!");
		return info;
	}
	else
	{
		info += ":";
		info += query_method;
		info += ":";
		info += text;
	}
	return info;
}
/* ************************************************
				查询信息按钮
************************************************ */

function on_button_query()
{
	var cmd = get_query_info();
	if(cmd == "1")
		return;
	var cmd1 = encodeURI(cmd);
	var URL = "/cgi-bin/text.cgi?";
	URL += cmd1;
	//alert("URL===" +URL);
	js_communicate_with_cgi(URL, 1);		
}