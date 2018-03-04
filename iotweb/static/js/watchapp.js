var app = {
    'pauseDuration': 5000,
    'init': function () {

    },
    'go' : function(){
        app.getAll();
        setInterval(app.getAll, app.pauseDuration);
    },
    'getAll' : function() {
        app.getRobotLocation();
        app.getStates();
        app.getTelemetries();
        app.getRobotCommands();
    },
    'getQueue': function (queueName, containerId) {
        var url = "/" + queueName + "/peek";
        var id = "#" + containerId;
        $.getJSON(
            url
        ).done(function (data) {
            $(id).html("");
            if (data.data.length > 0) {
                var colorIndex = 0;
                $.each(data.data, function (index, item) {
                    var colorCode = "#" + Array(7).join(colorIndex.toString(16));
                    colorIndex++;
                    var result = "<div class=\"row\"><div class=\"col-md\" style=\"color:"+colorCode+"\">" +
                        JSON.stringify(item) +
                        "</div></div>";
                    $(id).append(result);
                });
            }
        });
    },
    'getStates': function () {
        app.getQueue("state", "state-list");
    },
    'getTelemetries': function () {
        app.getQueue("telemetry", "telemetry-list");
    },
    'getRobotCommands': function () {
        app.getQueue("robotcommand", "robot-list");
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