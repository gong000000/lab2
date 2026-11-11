/*
 * 11번: DBus 알림 서버
 */
#include <dbus/dbus.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_NAME "org.example.Lab2Notify"
#define OBJECT_PATH "/org/example/Lab2Notify"
#define INTERFACE_NAME "org.example.Lab2Notify"

static volatile sig_atomic_t running = 1;

static void on_signal(int signo) {
    (void)signo;
    running = 0;
}

static DBusHandlerResult handle_message(DBusConnection *connection,
                                        DBusMessage *message,
                                        void *user_data) {
    (void)user_data;

    if (dbus_message_is_method_call(message, INTERFACE_NAME, "Notify")) {
        const char *text = NULL;
        DBusError error;
        dbus_error_init(&error);

        if (!dbus_message_get_args(message, &error,
                                   DBUS_TYPE_STRING, &text,
                                   DBUS_TYPE_INVALID)) {
            fprintf(stderr, "메시지 인자 오류: %s\n",
                    error.message != NULL ? error.message : "unknown");
            dbus_error_free(&error);
            return DBUS_HANDLER_RESULT_HANDLED;
        }

        printf("[알림] %s\n", text);
        fflush(stdout);

        DBusMessage *reply = dbus_message_new_method_return(message);
        if (reply == NULL) {
            fprintf(stderr, "응답 메시지를 만들 수 없습니다.\n");
            return DBUS_HANDLER_RESULT_HANDLED;
        }

        const char *response = "OK";
        if (!dbus_message_append_args(reply,
                                      DBUS_TYPE_STRING, &response,
                                      DBUS_TYPE_INVALID)) {
            fprintf(stderr, "응답 인자를 추가할 수 없습니다.\n");
            dbus_message_unref(reply);
            return DBUS_HANDLER_RESULT_HANDLED;
        }

        if (!dbus_connection_send(connection, reply, NULL)) {
            fprintf(stderr, "응답을 전송할 수 없습니다.\n");
        }

        dbus_message_unref(reply);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);

    (void)signal(SIGINT, on_signal);
    (void)signal(SIGTERM, on_signal);

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

    int result = dbus_bus_request_name(connection, SERVER_NAME,
                                       DBUS_NAME_FLAG_REPLACE_EXISTING,
                                       &error);
    if (dbus_error_is_set(&error)) {
        fprintf(stderr, "이름 등록 실패: %s\n", error.message);
        dbus_error_free(&error);
        dbus_connection_unref(connection);
        return EXIT_FAILURE;
    }

    if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "다른 프로세스가 이미 %s 이름을 사용 중입니다.\n", SERVER_NAME);
        dbus_connection_unref(connection);
        return EXIT_FAILURE;
    }

    DBusObjectPathVTable vtable;
    memset(&vtable, 0, sizeof(vtable));
    vtable.message_function = handle_message;

    if (!dbus_connection_register_object_path(connection, OBJECT_PATH,
                                              &vtable, NULL)) {
        fprintf(stderr, "객체 경로 등록 실패: %s\n", OBJECT_PATH);
        dbus_connection_unref(connection);
        return EXIT_FAILURE;
    }

    printf("DBus 알림 서버 시작\n");
    printf("  버스 이름: %s\n", SERVER_NAME);
    printf("  객체 경로: %s\n", OBJECT_PATH);
    printf("  인터페이스: %s\n", INTERFACE_NAME);
    printf("종료하려면 Ctrl-C를 누르세요.\n");
    fflush(stdout);

    while (running) {
        if (!dbus_connection_read_write_dispatch(connection, 200)) {
            fprintf(stderr, "DBus 연결이 끊겼습니다.\n");
            break;
        }
    }

    dbus_connection_unref(connection);
    printf("DBus 알림 서버 종료\n");
    return EXIT_SUCCESS;
}
