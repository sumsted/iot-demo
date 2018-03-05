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
        app.getSimon();
        app.getTouch();
        app.getHall();
        app.getWeb();
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
    'getSimon': function () {
        var url = "/state/peek/simon";
        app.getQueue(url, "simon-list");
    },
    'getHall': function () {
        var url = "/state/peek/hall";
        app.getQueue(url, "hall-list");
    },
    'getTouch': function () {
        var url = "/state/peek/touch";
        app.getQueue(url, "touch-list");
    },
    'getWeb': function () {
        var url = "/state/peek/web";
        app.getQueue(url, "web-list");
    },
    'getRobotCommands': function () {
        var url = "/robotcommand/peek";
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
