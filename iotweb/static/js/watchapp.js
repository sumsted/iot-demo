var app = {
    'pauseDuration': 5000,
    'init': function () {

    },
    'go' : function(){
        app.getAll();
        // setInterval(app.getAll, app.pauseDuration);
    },
    'getAll' : function() {
        app.getGatewayLog();
        app.getHubLog();
        app.getSubscriberLog();
        app.getSalesforceLog();
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
    'getHubLog': function () {
        var url = "/iot_hub_log/peek";
        app.getQueue(url, "hub-list");
    },
    'getSubscriberLog': function () {
        var url = "/iot_subscriber_log/peek";
        app.getQueue(url, "subscriber-list");
    },
    'getSalesforceLog': function () {
        var url = "/iot_salesforce_log/peek";
        app.getQueue(url, "salesforce-list");
    }
};

app.go();
