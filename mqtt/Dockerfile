FROM golang:latest AS build-stage

WORKDIR /src
COPY go.mod ./
RUN go mod download

COPY *.go ./
# lägga till fler mappar?

RUN CGO_ENABLED=0 GOOS=linux go build -o /server

FROM gcr.io/distroless/base-debian11 AS build-release-stage

WORKDIR /
COPY cert cert
COPY --from=build-stage /server /server

EXPOSE 8081

ENTRYPOINT [ "/server" ]