/**
  * file :   me.js
  * author : bushaofeng
  * create : 2015-10-24 18:02
  * func : 
  * history:
 */

var version = 0.1;

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
// (new Date()).Format("yyyy-MM-dd hh:mm:ss.S") ==> 2006-07-02 08:09:04.423 
// (new Date()).Format("yyyy-M-d h:m:s.S")      ==> 2006-7-2 8:9:4.18 
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

function BaseObject(className, o){
	objectMap = {};
	this.dirty = {};
	this.objectId = null;
	this.classname = className;

	this.put = function(k, v){
		if (this.objectId!=null && !('$set' in this.dirty)) {
			this.dirty['$set'] = {};
		}
		// 如果是BaseObject类型
		if (typeof v=='object' && ('objectId' in v) && ('getMeObject' in v)) {
			objectMap[k] = v;
			var pointer = {};
			pointer['_type'] = 'pointer';
			pointer['_class'] = v.classname;
			if (v.objectId!=null) {
				pointer['_id'] = v.objectId;
			}
			// 本身只存pointer
			this[k] = pointer;

			var p2 = pointer;
			p2['_content'] = v.dirty;
			if (this.objectId==null) {
				this.dirty[k] = p2;
			}
			else{
				this.dirty['$set'][k] = p2;
			}
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

function MeDevObject(className, obj){
	var o = new BaseObject(className, obj);
	o.save = function(){
		var defer = $.Deferred();

		if(this.objectId){
			var method = 'PUT';
			var url = "/dev/" + className+'/' + this.objectId;
		}
		else{
			var method = 'POST';
			var url = "/dev/" + className;
		}
		/*
		for(var key in o.dirty){
			o.dirty[key] = encodeURIComponent(o.dirty[key]);
		}
		*/
		$.ajax({
			url: url,
			type: method,
			data: JSON.stringify(o.dirty),
			//processData: false,
			dataType: 'json',
			success: function(data){
				o.copySelf(data);
				o.dirty = {};
				defer.resolve(o);
			},
			error: function(xhr, status, errorThrow){
				defer.reject(status);
			},
		});
		return defer;
	}
	o.delete = function(){
		var defer = $.Deferred();

		if(this.objectId){
			var method = 'DELETE';
			var url = "/dev/" + className+'/' + this.objectId;
		}
		else{
			defer.reject({'errCode':-1, 'errMsg':'无objectId'});
		}

		$.ajax({
			url: url,
			type: method,
			//processData: false,
			//dataType: 'json',
			success: function(data){
				defer.resolve(data);
			},
			error: function(xhr, status, errorThrow){
				defer.reject(status);
			},
		});
		return defer;
	}

	return o;
}

function MeObject(className, obj){
	var o = new BaseObject(className, obj);
	o.save = function(){
		var defer = $.Deferred();

		if(this.objectId){
			var method = 'PUT';
			var url = '/'+version+"/classes/" + className+'/' + this.objectId;
		}
		else{
			var method = 'POST';
			var url = '/'+version+"/classes/" + className;
		}

		$.ajax({
			url: url,
			type: method,
			data: JSON.stringify(o.dirty),
			processData: false,
			dataType: 'json',
			beforeSend: function (xhr) {
				if (appId!=null && appKey!=null) {
					xhr.setRequestHeader("X-MeCloud-AppId", appId);
					xhr.setRequestHeader("X-MeCloud-AppKey", appKey);
				}
			},
			success: function(data){
				o.copySelf(data);
				o.dirty = {};
				defer.resolve(o);
			},
			error: function(xhr, status, errorThrow){
				defer.reject(status);
			},
		});

		return defer;
	}

	return o;
}

function BaseQuery(className){
	this.where = {};

	this.whereEqualTo = function(k, v){
		this.where[k] = v;
	}
}

function MeDevQuery(className){
	var q = new BaseQuery(className);
	var url = '/dev/'+className;

	q.get = function(objectId){
		var defer = $.Deferred();

		$.ajax({
			url: url+'/'+objectId,
			type: 'GET',
			dataType: 'json',
			beforeSend: function (xhr) {
				if (appId!=null && appKey!=null) {
					xhr.setRequestHeader("X-MeCloud-AppId", appId);
					xhr.setRequestHeader("X-MeCloud-AppKey", appKey);
				}
			}
			success: function(data){
				if ('errCode' in data) {
					defer.reject(data);
				}
				else{
					var obj = new MeDevObject(className, data);
					defer.resolve(obj);
				}
			},
			error: function(xhr, status, errorThrow){
				defer.reject(xhr);
			}
		});
		return defer;
	}
	q.find = function(){
		var defer = $.Deferred();

		$.ajax({
			url: url+'?where='+JSON.stringify(q.where),
			type: 'GET',
			dataType: 'json',
			success: function(data){
				if ('errCode' in data) {
					defer.reject(data);
				}
				else{
					var list = [];
					for(var i=0; i<data.length; i++){
						var obj = new MeDevObject(className, data[i]);
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
	return q;
}

function MeQuery(className){
	var q = new BaseQuery(className);
	var url = '/'+version+'/classes/'+className;

	q.get = function(objectId){

	}
	q.find = function(){

	}
	return q;
}