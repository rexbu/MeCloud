/**
  * file :   me.js
  * author : bushaofeng
  * create : 2015-10-24 18:02
  * func : 
  * history:
 */

var version = '1.0';

var appId = null;
var appKey = null;

function MeCloud(){
	if ('appId' in localStorage && localStorage.appId) {
		appId = localStorage.appId;
		appKey = localStorage.appKey;
	}

	this.initialize = function(id, key){
		appId = id;
		appKey = key;
		localStorage.appId = id;
		localStorage.appKey = key;
	}
}

var Me = new MeCloud();
// 对Date的扩展，将 Date 转化为指定格式的String
// 月(M)、日(d)、小时(h)、分(m)、秒(s)、季度(q) 可以用 1-2 个占位符， 
// 年(y)可以用 1-4 个占位符，毫秒(S)只能用 1 个占位符(是 1-3 位的数字) 
// 例子： 
// (new Date()).format("yyyy-MM-dd hh:mm:ss.S") ==> 2006-07-02 08:09:04.423 
// (new Date()).format("yyyy-M-d h:m:s.S")      ==> 2006-7-2 8:9:4.18 
Date.prototype.format = function (fmt) { //author: meizz 
	var o = {
		"M+": this.getMonth() + 1, //月份 
		"d+": this.getDate(), //日 
		"h+": this.getHours(), //小时 
		"m+": this.getMinutes(), //分 
		"s+": this.getSeconds(), //秒 
		"q+": Math.floor((this.getMonth() + 3) / 3), //季度 
		"S": this.getMilliseconds() //毫秒 
	};
	if (/(y+)/.test(fmt)) fmt = fmt.replace(RegExp.$1, (this.getFullYear() + "").substr(4 - RegExp.$1.length));
	for (var k in o)
	if (new RegExp("(" + k + ")").test(fmt)) fmt = fmt.replace(RegExp.$1, (RegExp.$1.length == 1) ? (o[k]) : (("00" + o[k]).substr(("" + o[k]).length)));
	return fmt;
}

// JSON.prototype.encrypt = function(obj){
// 	var o = new Array()
// 	$.each(obj, function(k, v){
// 		o.push(encrypt(k)+'='+encrypt(v))
// 	});
// 	return o.join('&');
// }

// JSON.prototype.decrypt = function(){

// }


function str2UTF8(str){  
	var bytes = new Array();   
	var len,c;  
	len = str.length;  
	for(var i = 0; i < len; i++){  
		c = str.charCodeAt(i);  
		if(c >= 0x010000 && c <= 0x10FFFF){  
			bytes.push(((c >> 18) & 0x07) | 0xF0);  
			bytes.push(((c >> 12) & 0x3F) | 0x80);  
			bytes.push(((c >> 6) & 0x3F) | 0x80);  
			bytes.push((c & 0x3F) | 0x80);  
		}else if(c >= 0x000800 && c <= 0x00FFFF){  
			bytes.push(((c >> 12) & 0x0F) | 0xE0);  
			bytes.push(((c >> 6) & 0x3F) | 0x80);  
			bytes.push((c & 0x3F) | 0x80);  
		}else if(c >= 0x000080 && c <= 0x0007FF){  
			bytes.push(((c >> 6) & 0x1F) | 0xC0);  
			bytes.push((c & 0x3F) | 0x80);  
		}else{  
			bytes.push(c & 0xFF);  
		}  
	}  
	return bytes;  
}  

function byteToString(arr) {  
	if(typeof arr === 'string') {  
		return arr;  
	}  
	var str = '',  
		_arr = arr;  
	for(var i = 0; i < _arr.length; i++) {  
		var one = _arr[i].toString(2),  
			v = one.match(/^1+?(?=0)/);  
		if(v && one.length == 8) {  
			var bytesLength = v[0].length;  
			var store = _arr[i].toString(2).slice(7 - bytesLength);  
			for(var st = 1; st < bytesLength; st++) {  
				store += _arr[st + i].toString(2).slice(2);  
			}  
			str += String.fromCharCode(parseInt(store, 2));  
			i += bytesLength - 1;  
		} else {  
			str += String.fromCharCode(_arr[i]);  
		}  
	}  
	return str;  
}  

function encrypt(input){
    return input;
	input = str2UTF8(input)
	var output = "";
	var inSize = input.length;
	var bit = 0;
	for(var i=0;i<inSize;i++){
		bit++;
		var c = input[i] + Math.floor(bit/5+bit%3);
		output += c.toString(16);
	}
	return output;
}

function decrypt(input){
    return input;
	var output = new Array();
	var inSize = input.length;
	var bit = 0;
	for(var i=0;i<inSize;i+=2){
		bit++;
		var c = parseInt(input[i]+input[i+1],16);
		c -= Math.floor(bit/5+bit%3);
		output.push(c)
	}
	return byteToString(output)
}


function JSONEncrypt(obj){
	var o = new Array()
	$.each(obj, function(k, v){
		o.push(encrypt(k)+'='+encrypt(v))
	});
	return o.join('&');
};

function isEmpty(o){
	if(typeof o == 'object'){
		for(var k in o){
			return false;
		}
		return true;
	}
	else if(o===undefined || o===null){
		return true;
	}

	return false;
}

function MeObject(className, o){
	objectMap = {};
	this.dirty = {};
	this.objectId = null;
	this.classname = className;

	// 添加内容
	this.put = function(k, v){
		if (this.objectId!=null && !('$set' in this.dirty)) {
			this.dirty['$set'] = {};
		}
		// 如果是BaseObject类型
		if (typeof v=='object' && ('objectId' in v) && ('getMeObject' in v)) {
			//// TODO: 走/1.0/classes 

		}
		else{
			this[k] = v;
			if (this.objectId==null) {
				this.dirty[k] = v;
			}
			else{
				this.dirty['$set'][k] = v;
			}
		}
	}

	this.increase = function(k, v){
		var val = 1;
		if (v || v===0) {
			val = v;
		}
		this[k] += val;
		if (this.objectId) {
			if (!('$inc' in this.dirty)) {
				this.dirty['$inc'] = {}
			}
			this.dirty['$inc'][k] += val;
		}
		else{
			this.dirty[k] += val;
		}
	}

	this.get = function(key){
		return this[key];
	}
	this.getMeObject = function(key){
		if (key in objectMap) {
			return objectMap[key];
		}
		return null;
	}

	this.save = function(){
		var defer = $.Deferred();
		// 有objectId走更新，无走新建
		if(this.objectId){
			var method = 'PUT';
			var url = '/'+version+"/classV2/" + className+'/' + this.objectId;
		}
		else{
			var method = 'POST';
			var url = '/'+version+"/classV2/" + className;
		}

		$.ajax({
			url: url,
			type: method,
			data: encrypt(JSON.stringify(this.dirty)),
			processData: false,
			dataType: 'json',
			headers:{
			    'X-MeCloud-Debug':1
			},
			beforeSend: function (xhr) {
				// http头
				if (appId != null){
					xhr.setRequestHeader("X-MeCloud-AppId", appId);
					xhr.setRequestHeader("X-MeCloud-AppKey", appKey);
				}
				xhr.setRequestHeader("X-MeCloud-Version", version);
			},
			success: function(data){
			    defer.resolve(data);
//				o.copySelf(decrypt(data));
//				o.dirty = {};
//				defer.resolve(o);
			},
			error: function(xhr, status, errorThrow){
				defer.reject(status);
			},
		});

		return defer;
	}

	this.setACL = function(acl){
		put('acl', acl);
	}
	this.copySelf = function(obj){
		if (! (typeof obj=='object')){
			return false;
		}

		if('_id' in obj){
			this.objectId = obj['_id'];
		}

		var parent = this;
		$.each(obj, function(k, v){
			if ((k=='createAt' || k=='updateAt') && (typeof v == 'string')) {
				v = v.replace(/-/g,"/");
				parent[k] = new Date(v);
			}
			else if(k in objectMap){
				//TODO
				var o = this.getMeObject(k);
				o.dirty = {};
				o.copySelf(obj[key]['_content']);
				// id设置
				if (! '_id' in this[k]) {
					parent[k]['_id'] = o['_id'];
				}
			}
			else if((typeof v=='object') && ('_type' in v) && (v['_type']=='pointer') && ('_id' in v)){
				if ('_content' in v) {
					var mo = new BaseObject(v['_class']);
					mo.copySelf(v['_content']);
					objectMap[k] = mo;
				}
				else{
					var mo = new BaseObject(v['_class']);
					mo.objectId = v['_id'];
					objectMap[k] = mo;
				}
			}
			else{
				parent[k] = v;
			}
		});
		return true;
	}

	this.toString = function(){
		var json = {};
		$.each(this, function(k, v){
			var type = typeof v;
			if (k=='objectId' || k=='dirty' || k=='objectMap' || k=='classname' || type=='function') {
				return;
			}

			if (type=='object' && v instanceof Date) {
				json[k] = v.format("yyyy-MM-dd hh:mm:ss:S");
			}
			else if(k in objectMap){
				var obj = this.getMeObject(k);
				var j = {};
				j['_type'] = 'pointer';
				j['_class'] = obj.classname;
				j['_content'] = obj.toString();
				json[k] = obj;
			}
			else{
				json[k] = v;
			}
		});

		return JSON.stringify(json);
	}

	if(typeof o=='object'){
		this.copySelf(o);
	}
	else{
		this['createAt'] = new Date();
		this['updateAt'] = this['createAt'];
	}
}

function MeQuery(className){
    this.limit = 20;
    this.skip = 0;

	this.url = "/"+version+"/classV2/"+className;
	this.where = {};
	this.sort = {};
	this.keys = {};

	this.whereEqualTo = function(k, v){
		this.where[k] = v;
	}
	this.sortEqualTo = function(k, v){
		this.sort[k] = v;
	}
	this.keysEqualTo = function(k, v){
		this.keys[k] = v;
	}
	this.aggregate= ''
	this.aggregateEqualTo = function(k, v){
		this.aggregate[k] = v;
	}
	this.get = function(objectId){
		var defer = $.Deferred();
		$.ajax({
			url: this.url+'?where={"_id":"' + objectId + '"}',
			type: 'GET',
			dataType: 'text',
			headers:{
			    'X-MeCloud-Debug':1
			},
			success: function(data){
				data = JSON.parse(decrypt(data))
				if ('errCode' in data) {
					defer.reject(data);
				}
				else{
					var obj = new MeObject(className, data);
					defer.resolve(obj);
				}
			},
			error: function(xhr, status, errorThrow){
				defer.reject(xhr);
			}
		});
		return defer;
	}

	this.find = function(){
	    var defer = $.Deferred();
	    var thisUrl =  this.url+'?where='+JSON.stringify(this.where)+'&limit='+this.limit+'&skip='+this.skip;
	    if(JSON.stringify(this.keys)!="{}"){
	        var thisUrl =  thisUrl+'&keys='+JSON.stringify(this.keys);
	    }
	    if(JSON.stringify(this.sort)!="{}"){
            var thisUrl =  thisUrl+'&sort='+ JSON.stringify(this.sort);
	    }
        if(this.aggregate != ''){
             var thisUrl =  thisUrl+ '&aggregate='+this.aggregate
        }
		$.ajax({
			 url:thisUrl,
			type: 'GET',
			dataType: 'text',
			headers:{
			    'X-MeCloud-Debug':1
			},
			success: function(data){
				data = JSON.parse(decrypt(data))
				if ('errCode' in data) {
					defer.reject(data);
				}
				else{
					var list = [];
					for(var i=0; i<data.length; i++){
						var obj = new MeObject(className, data[i]);
						list.push(obj)
					}
					defer.resolve(list);
				}
			},
			error: function(xhr, status, errorThrow){
				defer.reject(xhr);
			}
		});
		return defer;
	}
}

function MeUser(){
	var o = new MeObject("User");
	o.login = function(username, password){
		data = {
			'username':username,
			'password':password
		};
		var defer = $.Deferred();
		url = '/'+version+'/user/login';
		$.ajax({
			url: url,
			type: 'POST',
			data: encrypt(JSON.stringify(data)),
			processData: false,
			dataType: 'text',
			headers:{
			    'X-MeCloud-Debug':1
			},
			beforeSend: function (xhr) {
				xhr.setRequestHeader("X-MeCloud-Version", '1.0');
			},
			success: function(data){
				console.log(decrypt(data))
				o.copySelf(JSON.parse(decrypt(data)));
				o.dirty = {};
				defer.resolve(o);
				var Data = JSON.parse(data)
				if('errCode' in Data){
                   alert( Data.errMsg)
				}

			},
			error: function(xhr, status, errorThrow){
				defer.reject(status);
			},
		});
		return defer
	}
	return o;
}

function MeRole(){
	var o = new MeObject("Role");
	return o;
}

function QueryCount(){
	this.url = "/"+version+"/query/"
	this.whereEqualTo='';
	this.count = function(){
		var defer = $.Deferred();
		$.ajax({
		    url: this.url+'?where='+this.whereEqualTo,
			type: 'GET',
			dataType: 'text',
			headers:{
			    'X-MeCloud-Debug':1
			},
			success: function(data){
				data = JSON.parse(decrypt(data))
				if ('errCode' in data) {
					defer.reject(data);
				}
				else{
					defer.resolve(data);
				}
			},
			error: function(xhr, status, errorThrow){
				defer.reject(xhr);
			}
		});
		return defer;
	}
}

function MeACL(){

}
function followAjax(posturl,data,func,errorFunc){
    $.ajax({
        url: '/1.0/follow'+posturl,
        type:'get',
        dataType:'json',
        data:data,
        headers:{
            'X-MeCloud-Debug':1
        },
        success:function(result){
            try{
                func(result);
            } catch(e){
//		                console.log(e);
            }
        },
        complete:function(){
        },
        error: function (){
        }
    });
}