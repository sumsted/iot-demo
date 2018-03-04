var app = {
    'pauseDuration': 5000,
    'init': function () {

    },
    'go' : function(){
        setInterval(app.getAll, app.pauseDuration);
    },
    'getAll' : function() {
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
                $.each(data.data, function (index, item) {
                    var result = "<div class=\"row\"><div class=\"col-md\" style=\"color:#444\">" +
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
    }
}

app.go();
