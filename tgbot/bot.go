package main

import (
	"context"
	"errors"
	"fmt"
	tgbotapi "github.com/skinass/telegram-bot-api/v5"
	"log"
	"net/http"
	"os"
	"strconv"
	"strings"
)

var (
	BotToken = "5213969537:AAESi4FodA3i4Tj5CTpBDt-AMnAiF6Eob9E"

	WebhookURL = "https://5693-46-31-27-151.ngrok.io"
)

var id = 1

const (
	WrongMessage = "Неверное сообщение"
	NoTasks      = "Нет задач"
	WrongID      = "Inappropriate ID"
)

type Task struct {
	ID             int
	taskName       string
	ownerName      string
	executor       string
	ownerChatID    int64
	executorChatID int64
}

func (task Task) isProcessing() bool {
	return task.executor != "None"
}

func sendMessage(bot *tgbotapi.BotAPI, update tgbotapi.Update, str string) {
	_, err := bot.Send(tgbotapi.NewMessage(
		update.Message.Chat.ID,
		str,
	))
	if err != nil {
		log.Printf("Error occured while sending message: %v", err)
	}
}

func commandCorrectType1(command []string, tasks []Task, bot *tgbotapi.BotAPI, update tgbotapi.Update) bool {
	if len(command) != 1 {
		sendMessage(bot, update, WrongMessage)
		return false
	}
	if len(tasks) == 0 {
		sendMessage(bot, update, NoTasks)
		return false
	}
	return true
}

func commandCorrectType2(command []string, tasks []Task, bot *tgbotapi.BotAPI, update tgbotapi.Update) (int, bool) {
	if len(tasks) == 0 {
		sendMessage(bot, update, NoTasks)
		return 0, false
	}
	if len(command) != 2 {
		sendMessage(bot, update, WrongMessage)
		return 0, false
	}
	idd, err := strconv.Atoi(command[1])
	if len(command) != 2 || err != nil || idd > id-1 || idd < 0 {
		sendMessage(bot, update, WrongID)
		return 0, false
	}
	return idd, true
}

func tasksHandle(command []string, tasks []Task, bot *tgbotapi.BotAPI, update tgbotapi.Update) {
	if !commandCorrectType1(command, tasks, bot, update) {
		return
	}
	str := ""
	for _, i := range tasks {
		if !i.isProcessing() {
			str = str + strconv.Itoa(i.ID) + ". " + i.taskName + " by @" + i.ownerName + "\n/assign_" + strconv.Itoa(i.ID) + "\n\n"
		} else {
			if i.executor == update.Message.From.UserName {
				str = str + strconv.Itoa(i.ID) + ". " + i.taskName + " by @" + i.ownerName + "\nassignee: я\n/unassign_" + strconv.Itoa(i.ID) +
					" /resolve_" + strconv.Itoa(i.ID) + "\n\n"
			} else {
				str = str + strconv.Itoa(i.ID) + ". " + i.taskName + " by @" + i.ownerName + "\nassignee: @" + i.executor + "\n\n"
			}
		}
	}
	str = str[:len(str)-2]
	sendMessage(bot, update, str)
}

func myHandle(command []string, tasks []Task, bot *tgbotapi.BotAPI, update tgbotapi.Update) {
	if !commandCorrectType1(command, tasks, bot, update) {
		return
	}
	str := ""
	for _, i := range tasks {
		if i.executor == update.Message.From.UserName {
			str = str + strconv.Itoa(i.ID) + ". " + i.taskName + " by @" + i.ownerName + "\n/unassign_" + strconv.Itoa(i.ID) +
				" /resolve_" + strconv.Itoa(i.ID) + "\n\n"
		}
	}
	if str == "" {
		sendMessage(bot, update, NoTasks)
		return
	}
	str = str[:len(str)-2]
	sendMessage(bot, update, str)
}

func ownerHandle(command []string, tasks []Task, bot *tgbotapi.BotAPI, update tgbotapi.Update) {
	if !commandCorrectType1(command, tasks, bot, update) {
		return
	}
	str := ""
	for _, i := range tasks {
		if i.ownerName == update.Message.From.UserName {
			if !i.isProcessing() {
				str = str + strconv.Itoa(i.ID) + ". " + i.taskName + " by @" + i.ownerName + "\n/assign_" + strconv.Itoa(i.ID) + "\n\n"
			} else {
				str = str + strconv.Itoa(i.ID) + ". " + i.taskName + " by @" + i.ownerName + "\n/unassign_" + strconv.Itoa(i.ID) +
					" /resolve_" + strconv.Itoa(i.ID) + "\n\n"
			}
		}
	}
	if str == "" {
		sendMessage(bot, update, NoTasks)
		return
	}
	str = str[:len(str)-2]
	sendMessage(bot, update, str)
}

func newHandle(command []string, tasks []Task, bot *tgbotapi.BotAPI, update tgbotapi.Update) []Task {
	if len(command) == 1 {
		sendMessage(bot, update, WrongMessage)
		return tasks
	}
	name := strings.Join(command[1:], " ")
	tasks = append(tasks, Task{
		ID:             id,
		taskName:       name,
		ownerName:      update.Message.From.UserName,
		executor:       "None",
		ownerChatID:    update.Message.From.ID,
		executorChatID: 0,
	})
	str := "Задача " + "\"" + name + "\"" + " создана, id=" + strconv.Itoa(id)
	id++
	sendMessage(bot, update, str)
	return tasks
}

func assignHandle(command []string, tasks []Task, bot *tgbotapi.BotAPI, update tgbotapi.Update) []Task {
	idd, err1 := commandCorrectType2(command, tasks, bot, update)
	if !err1 {
		return tasks
	}
	str := ""
	var index int
	flag := false
	for j, i := range tasks {
		if idd == i.ID {
			index = j
			flag = true
		}
	}
	if !flag {
		sendMessage(bot, update, WrongID)
		return tasks
	}
	tasks[index].executor = update.Message.From.UserName
	str = "Задача " + "\"" + tasks[index].taskName + "\"" + " назначена на вас"
	sendMessage(bot, update, str)
	if tasks[index].executorChatID != 0 {
		str = "Задача " + "\"" + tasks[index].taskName + "\"" + " назначена на @" + tasks[index].executor
		_, err := bot.Send(tgbotapi.NewMessage(
			tasks[index].executorChatID,
			str,
		))
		if err != nil {
			fmt.Println("Error occured while sending message")
			return tasks
		}
	}
	tasks[index].executorChatID = update.Message.Chat.ID
	return tasks
}

func unassignHandle(command []string, tasks []Task, bot *tgbotapi.BotAPI, update tgbotapi.Update) []Task {
	idd, err1 := commandCorrectType2(command, tasks, bot, update)
	if !err1 {
		return tasks
	}
	str := ""
	var index int
	flag := false
	for j, i := range tasks {
		if idd == i.ID {
			index = j
			flag = true
		}
	}
	if !flag {
		sendMessage(bot, update, WrongID)
		return tasks
	}
	if tasks[index].executor != update.Message.From.UserName {
		str = "Задача не на вас"
		sendMessage(bot, update, str)
		return tasks
	}
	if !tasks[index].isProcessing() {
		str = "Задача не выполняется"
		sendMessage(bot, update, str)
		return tasks
	}
	str = "Принято"
	sendMessage(bot, update, str)
	if tasks[index].executor != tasks[index].ownerName {
		str = "Задача " + "\"" + tasks[index].taskName + "\"" + " осталась без исполнителя"
		_, err := bot.Send(tgbotapi.NewMessage(
			tasks[index].ownerChatID,
			str,
		))
		if err != nil {
			fmt.Println("Error occured while sending message")
		}
	}
	tasks[index].executorChatID = tasks[index].ownerChatID
	tasks[index].executor = "None"
	return tasks
}

func resolveHandle(command []string, tasks []Task, bot *tgbotapi.BotAPI, update tgbotapi.Update) []Task {
	idd, err1 := commandCorrectType2(command, tasks, bot, update)
	if !err1 {
		return tasks
	}
	str := ""
	var index int
	flag := false
	for j, i := range tasks {
		if idd == i.ID && i.executor == update.Message.From.UserName {
			index = j
			flag = true
		}
	}
	if !flag {
		sendMessage(bot, update, WrongID)
		return tasks
	}
	str = "Задача " + "\"" + tasks[index].taskName + "\"" + " выполнена"
	sendMessage(bot, update, str)
	if tasks[index].executor != tasks[index].ownerName {
		str = "Задача " + "\"" + tasks[index].taskName + "\"" + " выполнена @" + tasks[index].executor
		_, err := bot.Send(tgbotapi.NewMessage(
			tasks[index].ownerChatID,
			str,
		))
		if err != nil {
			fmt.Println("Error occured while sending message")
		}
	}
	if str == "" {
		sendMessage(bot, update, WrongID)
		return tasks
	}
	tasks[index] = tasks[len(tasks)-1]
	tasks = tasks[:len(tasks)-1]
	return tasks
}

func startTaskBot(ctx context.Context) error {
	_ = ctx
	bot, err := tgbotapi.NewBotAPI(BotToken)
	if err != nil {
		return errors.New("NewBotAPI failed")
	}
	wh, err := tgbotapi.NewWebhook(WebhookURL)
	if err != nil {
		return errors.New("NewWebhook failed")
	}
	_, err = bot.Request(wh)
	if err != nil {
		return errors.New("SetWebhook failed")
	}
	updates := bot.ListenForWebhook("/")
	port := os.Getenv("PORT")
	if port == "" {
		port = "8080"
	}
	go func() {
		log.Fatalln("http err:", http.ListenAndServe(":"+port, nil))
	}()
	fmt.Println("start listen :" + port)
	tasks := make([]Task, 0)
	for update := range updates {
		log.Printf("upd: %#v\n", update)
		if update.Message == nil || update.Message.From == nil {
			continue
		}
		fmt.Println(update.Message.Text)
		command := strings.Split(update.Message.Text, " ")
		switch command[0] {
		case "/tasks":
			tasksHandle(command, tasks, bot, update)
			continue

		case "/my":
			myHandle(command, tasks, bot, update)
			continue

		case "/owner":
			ownerHandle(command, tasks, bot, update)
			continue

		case "/new":
			tasks = newHandle(command, tasks, bot, update)
			continue

		default:
		}
		command = strings.Split(update.Message.Text, "_")
		switch command[0] {
		case "/assign":
			tasks = assignHandle(command, tasks, bot, update)
			continue

		case "/unassign":
			tasks = unassignHandle(command, tasks, bot, update)
			continue

		case "/resolve":
			tasks = resolveHandle(command, tasks, bot, update)
			continue

		default:
			sendMessage(bot, update, WrongMessage)
		}
	}
	return nil
}

func main() {
	err := startTaskBot(context.Background())
	if err != nil {
		panic(err)
	}
}
