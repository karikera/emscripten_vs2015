mergeInto(LibraryManager.library, {
	canvasCreate: function ()
	{
		var canvas = document.getElementById("canvas");
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
		window.ctx = canvas.getContext("2d");
	},
	canvasClear: function()
	{
		ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
	},
	canvasBeginPath: function ()
	{
		ctx.beginPath();
	},
	canvasMoveTo: function(x, y)
	{
		ctx.moveTo(x, y);
	},
	canvasLineTo: function (x, y)
	{
		ctx.lineTo(x, y);
	},
	canvasStroke: function ()
	{
		ctx.stroke();
	},
	showDevTools: function()
	{
		try
		{
			require('nw.gui').Window.get().showDevTools();
		}
		catch (e)
		{
			// Without node.js
		}
	},
});