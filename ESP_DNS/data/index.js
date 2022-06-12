//-----------------------------------------------------------------------------
var app = {
	"debug": false,
	"new":{
		"domain": "",
		"ip": "",
	},
};

//-----------------------------------------------------------------------------
function makeHttpObject()
{
	try {return new XMLHttpRequest();}
	catch (error) {}
	try {return new ActiveXObject("Msxml2.XMLHTTP");}
	catch (error) {}
	try {return new ActiveXObject("Microsoft.XMLHTTP");}
	catch (error) {}
	throw new Error("Could not create HTTP request object.");
}

//-----------------------------------------------------------------------------
window.onload = function(){
	if( document.location.pathname == "/" ){
		buildUI();
	}
};



//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
function buildUI()
{
	let obj = document.getElementById( 'contentBox' );
	if( obj == undefined ){
		console.error( "contentBox is undefined" );
		return;
	}

	var request = makeHttpObject();
	request.open( "GET", "/rules", true );
	request.send( null );

	request.onreadystatechange = function(){
		if( request.readyState == 4 ){
			if( request.status == 200 ){
				if( app.debug ) console.log( "buildUI >:", request.responseText );
				let dataObject = JSON.parse( request.responseText );

				obj.innerHTML = "";

				let table = document.createElement( "table", );
				table.innerHTML = '<tr><th>Domain</th><th>IP</th><th>Actions</th></tr>';
				obj.appendChild( table );

				for( let num in dataObject ){
					let data = dataObject[ num ]; 
					if( !data.hasOwnProperty( "domain" ) ) continue;
					if( !data.hasOwnProperty( "ip" ) ) continue;
					let tr = document.createElement( "tr", );
					tr.innerHTML = '<td>' + data.domain + '</td><td>' + data.ip + '</td><td>*</td>';
					table.appendChild( tr );
				}

				table.innerHTML += '<tr><th><input type="text" placeholder="domain name" lang="dname" onInput="newRuleProcess( event, this );"></th><th><input type="text" placeholder="IP address" lang="ipaddr" onInput="newRuleProcess( event, this );"></th><th><input type="button" value="ADD" onClick="newRuleProcess( event, this );"></th></tr>';
			}
		}
	};
}

//-----------------------------------------------------------------------------
function newRuleProcess( event, inputObject = undefined )
{
	if( app.debug ) console.log( "newRuleProcess >: ", event, inputObject );

	if( inputObject == undefined ){
		console.error( "inputObject is undefined" );
		return;
	}

	if( inputObject.tagName == undefined ){
		console.error( "inputObject.tagName is undefined" );
		return;
	}

	if( inputObject.type == undefined ){
		console.error( "inputObject.type is undefined" );
		return;
	}

	if( inputObject.lang == undefined ){
		console.error( "inputObject.lang is undefined" );
		return;
	}

	if( inputObject.value == undefined ){
		console.error( "inputObject.value is undefined" );
		return;
	}

	if( inputObject.tagName != "INPUT" ){
		console.error( "inputObject.tagName is not input" );
		return;
	}

	switch( inputObject.type ){
		case "text":
			if( inputObject.lang == "dname" ){
				app.new.domain = inputObject.value;
				// if() focusNextTableElement( inputObject );
			}else if( inputObject.lang == "ipaddr" ){
				app.new.ip = inputObject.value;
			}
		break;
		case "button":
			var request = makeHttpObject();
			var sendForm = new FormData();
			request.open( "POST", "/rules", true );
			sendForm.append( 'setRule', app.new.domain );
			sendForm.append( 'ip', app.new.ip );
			request.onreadystatechange = function(){
				if( request.readyState == 4 ){
					if( request.status == 200 ){
						buildUI();
					}
				}
			};
			request.send( sendForm );
		break;
	}
}

//-----------------------------------------------------------------------------
function CheckBit( reg, bit ) 
{
	reg = Number( reg ).toFixed();
	bit = Number( bit ).toFixed();
	return ( ( reg & (1<<bit ) ) != 0 ) ? 1 : 0;
}

//-----------------------------------------------------------------------------
function focusNextTableElement( element )
{
	if( element == undefined ) return;
	if( element.parentNode == undefined ) return;
	if( element.parentNode == null ) return;
	if( element.parentNode.nextElementSibling == undefined ) return;
	if( element.parentNode.nextElementSibling == null ) return;
	if( element.parentNode.nextElementSibling.firstElementChild == undefined ) return;
	if( element.parentNode.nextElementSibling.firstElementChild == null ) return;

	element.parentNode.nextElementSibling.firstElementChild.focus();
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
