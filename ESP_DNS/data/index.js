//-----------------------------------------------------------------------------
var app = {
	"debug": false,
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
	request.open( "GET", "/getRules", true );
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
			}
		}
	};
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
function CheckBit( reg, bit ) 
{
	reg = Number( reg ).toFixed();
	bit = Number( bit ).toFixed();
	return ( ( reg & (1<<bit ) ) != 0 ) ? 1 : 0;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
