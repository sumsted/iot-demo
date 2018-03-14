var app = {
    'pauseDuration': 5000,
    'init': function () {

    },
    'go' : function(){
        app.getAll();
        setInterval(app.getAll, app.pauseDuration);
    },
    'getAll' : function() {
        app.getGatewayLog();
        app.getWebHookLog();
        app.getRobotLocation();
        app.getRobotCommands();
    },
    'getQueue': function (url, containerId) {
        var id = "#" + containerId;
        $.getJSON(
            url
        ).done(function (data) {
            $(id).html("");
            if (data.data.length > 0) {
                var colorIndex = 0;
                $.each(data.data, function (index, item) {
                    var colorCode = "#" + Array(7).join(colorIndex.toString(16));
                    colorIndex+=2;
                    var result = "<div class=\"row\"><div class=\"col-md\" style=\"color:"+colorCode+"\">" +
                        JSON.stringify(item) +
                        "</div></div>";
                    $(id).append(result);
                });
            }
        });
    },
    'getGatewayLog': function () {
        var url = "/iot_gateway_log/peek";
        app.getQueue(url, "gateway-list");
    },
    'getWebHookLog': function () {
        var url = "/iot_web_hook_log/peek";
        app.getQueue(url, "connector-list");
    },
    'getRobotCommands': function () {
        var url = "/robot/peek";
        app.getQueue(url, "robot-list");
    },
    'getRobotLocation': function() {
        var url = "/robot/location";
        $.getJSON(
            url
        ).done(function (data) {
            $("#robot-location").html(
                "<a target=\"_blank\" class=\"robot-location-a\" href=\"" + data.data.location + "\">" + data.data.location + "</a>"
            );
        });
    }
}

app.go();
