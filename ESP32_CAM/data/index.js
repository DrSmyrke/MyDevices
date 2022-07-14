//-----------------------------------------------------------------------------
var app = {
	"debug": false,
	"version": 99999,
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
		buildIndexPage();
	}

	updateData();
};



//-----------------------------------------------------------------------------
function updateCaptureImg()
{
	let img = document.getElementById( 'captureImg' );
	if( img == undefined ){
		console.error( "captureImg is undefined" );
		return;
	}

	let timestamp = new Date().getTime();
	img.src = 'http://' + document.location.hostname  + '/capture?t=' + timestamp;
}

//-----------------------------------------------------------------------------
function buildIndexPage()
{
	let obj = document.getElementById( 'contentBox' );
	if( obj == undefined ){
		console.error( "contentBox is undefined" );
		return;
	}
	
	obj.innerHTML = '<div class="camBox"><fieldset><legend>Capture <a href="javascript:void( 0 );" onClick="updateCaptureImg();">Update</a></legend><img id="captureImg" src="http://' + document.location.hostname  + '/capture"></fieldset><fieldset><legend>Stream</legend><img src="http://' + document.location.hostname  + ':81/stream"></fieldset></div>';
}

//-----------------------------------------------------------------------------
function buildUpdatePage()
{
	window.stop();
	
	let obj = document.getElementById( 'contentBox' );
	if( obj == undefined ){
		console.error( "contentBox is undefined" );
		return;
	}

	obj.innerHTML = '<input type="button" value="Check update" onClick="checkUpdate();"> <span id="newVersionL"><span>';
}

//-----------------------------------------------------------------------------
function updateData()
{

	var request = makeHttpObject();
	request.open( "GET", "/get", true );
	request.send( null );

	request.onreadystatechange = function(){
		if( request.readyState == 4 ){
			if( request.status == 200 ){
				if( app.debug ) console.log( "updateData >:", request.responseText );
				let dataObject = JSON.parse( request.responseText );

				if( dataObject.hasOwnProperty( "version" ) ){
					let vLabel = document.getElementById( 'versionL' );
					if( vLabel != undefined ){
						vLabel.innerHTML = "v" + dataObject.version;
						app.version = Number( dataObject.version ).toFixed();
					}
				}
			}
		}
	};
}

//-----------------------------------------------------------------------------
function checkUpdate()
{
	let obj = document.getElementById( 'newVersionL' );
	if( obj == undefined ){
		console.error( "newVersionL is undefined" );
		return;
	}

	obj.innerHTML = 'Checking...';

	var request = makeHttpObject();
	request.open( "GET", "/update?cmd=check_update", true );
	request.send( null );

	request.onreadystatechange = function(){
		if( request.readyState == 4 ){
			if( request.status == 200 ){
				if( app.debug ) console.log( "checkUpdate >:", request.responseText );
				let dataObject = JSON.parse( request.responseText );

				if( dataObject.hasOwnProperty( "newVersion" ) ){
					obj.innerHTML = "Firmvare available: v" + dataObject.newVersion;
					let newVersion = Number( dataObject.newVersion ).toFixed();
					if( newVersion > app.version ){
						obj.innerHTML += ' <input type="button" value="UPDATE" onClick="updateDevice();">';
					}
				}
			}else if( request.status == 401 ){
				obj.innerHTML = request.responseText;
			}else{
				obj.innerHTML = "Unknown Error";
			}
		}
	};
}

//-----------------------------------------------------------------------------
function updateDevice()
{
	let obj = document.getElementById( 'contentBox' );
	if( obj == undefined ){
		console.error( "contentBox is undefined" );
		return;
	}

	obj.innerHTML = 'Updating...';

	var request = makeHttpObject();
	request.open( "GET", "/update?cmd=update", true );
	request.send( null );

	request.onreadystatechange = function(){
		if( request.readyState == 4 ){
			if( request.status == 200 ){
				if( app.debug ) console.log( "updateDevice >:", request.responseText );
				obj.innerHTML = "Success";
				document.location = '/';
			}
		}else if( request.status == 401 ){
			obj.innerHTML = request.responseText;
		}else{
			obj.innerHTML = "Unknown Error [" + request.status + "]";
		}
	};
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
