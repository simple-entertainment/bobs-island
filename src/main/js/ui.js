
var exactFrameTimes = new Map();
var frameTimes = new Map();
var graphBorderThickness = 2;
var graphBorderOffset = graphBorderThickness / 2;
var graphScale = 30000;
var graphTops = [];

$(document).ready(function()
{
	$("#toolbar-background").slideDown();
	$("#toolbar").slideDown();
	$("#timerGraph").attr("width", window.innerWidth * 0.8);

    advance();
});

function advance()
{
    var canvas = $("canvas:visible");

	if (canvas.length === 0)
	{
		window.requestAnimationFrame(advance);
		return;
	}

	var background = new Image(8, 8);
	background.src = "../../../assets/diagonal.png";

	var context = canvas.get(0).getContext("2d");
	context.clearRect(0, 0, canvas.width(), canvas.height());
	context.fillStyle = context.createPattern(background, "repeat");
	context.fillRect(0, 0, canvas.width(), canvas.height());
	/*context.beginPath();
	context.moveTo(graphBorderOffset, graphBorderOffset);
	context.lineTo(canvas.width() - graphBorderOffset, graphBorderOffset);
	context.lineTo(canvas.width() - graphBorderOffset, canvas.height() - graphBorderOffset);
	context.lineTo(graphBorderOffset, canvas.height() - graphBorderOffset);
	context.closePath();
	context.stroke();
	context.fill();*/

    if (frameTimes.get("total") === undefined)
	{
		window.requestAnimationFrame(advance);
		return;
	}

	graphTops = [];
	for (var index = 0; index < canvas.width(); index++)
	{
		graphTops.push(canvas.height() - graphBorderOffset);
	}
	context.lineWidth = 2;

	context.strokeStyle = "#08e";
	drawFrameTimes(canvas, context, frameTimes.get("engine0"));
	context.strokeStyle = "#0e8";
	drawFrameTimes(canvas, context, frameTimes.get("engine1"));
	context.strokeStyle = "#e80";
	drawFrameTimes(canvas, context, frameTimes.get("engine2"));
	context.strokeStyle = "#e08";
	drawFrameTimes(canvas, context, frameTimes.get("engine3"));
	context.strokeStyle = "#08e";
	drawFrameTimes(canvas, context, frameTimes.get("engine4"));
	context.strokeStyle = "#0e8";
	drawFrameTimes(canvas, context, frameTimes.get("engine5"));
	context.strokeStyle = "#e80";
	drawFrameTimes(canvas, context, frameTimes.get("engine6"));

    window.requestAnimationFrame(advance);
}

function drawFrameTimes(canvas, context, frameTimes)
{
	var innerCanvasWidth = canvas.width() - graphBorderThickness;
    var frameCount = Math.min(innerCanvasWidth, frameTimes.length);
    var firstFrame = Math.max(0, frameTimes.length - innerCanvasWidth);
	var firstFrameOffset = graphBorderOffset;
	if (frameCount < innerCanvasWidth)
	{
		firstFrameOffset = innerCanvasWidth - frameCount + graphBorderOffset;
	}

    context.beginPath();
    context.moveTo(canvas.width() - graphBorderOffset, graphTops[0]);

    if (graphTops[0] === canvas.height() - graphBorderOffset)
    {
        context.lineTo(firstFrameOffset, graphTops[0]);
    }
    else
    {
        for (var index = frameCount - 1; index >= 0; index--)
        {
            context.lineTo(index + firstFrameOffset, graphTops[index]);
        }
    }

    for (var index = 0; index < frameCount; index++)
    {
        var graphPosition = graphTops[index] - frameTimes[firstFrame + index] * graphScale;
        context.lineTo(index + firstFrameOffset, graphPosition);
        graphTops[index] = graphPosition - graphBorderThickness - 5;
    }

    context.closePath();
    context.stroke();
    context.fill();
}

function toggle(sectionId)
{
	var section = $("#" + sectionId + ":visible");
	if (section.length === 1)
	{
		section.slideUp();
	}
	else
	{
		$("#" + sectionId).slideDown();
	}
}

function setFrameTime(name, time)
{
    if (exactFrameTimes.get(name) === undefined)
    {
        exactFrameTimes.set(name, [time]);
    }
    else
    {
        var myExactTimeFrames = exactFrameTimes.get(name);
        myExactTimeFrames.push(time);

        if (myExactTimeFrames.length > 10)
        {
            var averageTimeFrame = 0;
            for (var index = 0;index < myExactTimeFrames.length; index++)
            {
                averageTimeFrame += myExactTimeFrames[index];
            }
            averageTimeFrame /= 10;
            myExactTimeFrames.shift();

            if (frameTimes.get(name) === undefined)
            {
                frameTimes.set(name, [averageTimeFrame]);
            }
            else
            {
                frameTimes.get(name).push(averageTimeFrame);
            }
        }
    }
}
