/*
 * 11번: DBus 알림 클라이언트
 */
#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_NAME "org.example.Lab2Notify"
#define OBJECT_PATH "/org/example/Lab2Notify"
#define INTERFACE_NAME "org.example.Lab2Notify"

int main(int argc, char *argv[]) {
    const char *text = (argc > 1) ? argv[1] : "Hello from DBus client";

    if (argc > 2) {
        fprintf(stderr, "사용법: %s [메시지]\n", argv[0]);
        return EXIT_FAILURE;
    }

    DBusError error;
    dbus_error_init(&error);

    DBusConnection *connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (connection == NULL) {
        fprintf(stderr, "세션 버스 연결 실패: %s\n",
                error.message != NULL ? error.message : "unknown");
        dbus_error_free(&error);
        return EXIT_FAILURE;
    }

    dbus_connection_set_exit_on_disconnect(connection, FALSE);

    DBusMessage *message = dbus_message_new_method_call(
        SERVER_NAME, OBJECT_PATH, INTERFACE_NAME, "Notify");
    if (message == NULL) {
        fprintf(stderr, "메시지를 만들 수 없습니다.\n");
        dbus_connection_unref(connection);
        return EXIT_FAILURE;
    }

    if (!dbus_message_append_args(message,
                                  DBUS_TYPE_STRING, &text,
                                  DBUS_TYPE_INVALID)) {
        fprintf(stderr, "메시지 인자를 추가할 수 없습니다.\n");
        dbus_message_unref(message);
        dbus_connection_unref(connection);
        return EXIT_FAILURE;
    }

    DBusMessage *reply = dbus_connection_send_with_reply_and_block(
        connection, message, 5000, &error);

    dbus_message_unref(message);

    if (reply == NULL) {
        fprintf(stderr, "서버 호출 실패: %s\n",
                error.message != NULL ? error.message : "unknown");
        dbus_error_free(&error);
        dbus_connection_unref(connection);
        return EXIT_FAILURE;
    }

    const char *response = NULL;
    if (dbus_message_get_args(reply, &error,
                              DBUS_TYPE_STRING, &response,
                              DBUS_TYPE_INVALID)) {
        printf("서버 응답: %s\n", response);
    } else {
        printf("서버가 응답했지만 문자열을 읽지 못했습니다.\n");
        dbus_error_free(&error);
    }

    dbus_message_unref(reply);
    dbus_connection_unref(connection);

    printf("메시지 전송 완료: %s\n", text);
    return EXIT_SUCCESS;
}
