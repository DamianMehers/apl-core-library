/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifdef ALEXAEXTENSIONS
#ifndef _APL_EXTENSION_MEDIATOR_H
#define _APL_EXTENSION_MEDIATOR_H


#include <alexaext/alexaext.h>

#include "extensionclient.h"
#include "apl/content/rootconfig.h"
#include "apl/content/content.h"
#include "apl/engine/rootcontext.h"

namespace apl {

class RootContext;

/**
 * This class mediates message passing between "local" alexaext::Extension and the APL engine.  It
 * is intended for internal use by the viewhost. Remote extensions are not supported.
 *
 * ExtensionMediator is an experimental class requiring RootConfig::kExperimentalExtensionProvider. It
 * is expected to be eliminated before APL 2.0.  The class temporarily supports the following extension
 * message  processes:
 * - Registration: using the loadExtensions(...) API
 * - Commands: using the invokeCommand(,,) API
 * - Events: handled internally after registration, no outward API
 * - LiveData Updates: handled internally after registration, no outward API.
 *
 * The message executor allows for messages from the extension to be enqueued/sequenced before processing.
 * Any messag from the extension is passed through the enqueue(...) call.  Implementors should ensure message
 * processing is aligned with the overall APL execution model.
 *
 * This class cannot be used with more than one Document / RootContext.
 */
class ExtensionMediator: public std::enable_shared_from_this<ExtensionMediator> {

public:

    /**
     * @deprecated
     * Create a message mediator for the alexaext:Extensions registered with given alexaext::ExtensionProvider.
     * @param provider The extension provider.
     */
    static ExtensionMediatorPtr
    create(const alexaext::ExtensionProviderPtr& provider) {
        return std::make_shared<ExtensionMediator>(provider, alexaext::Executor::getSynchronousExecutor());
    }

    /**
     * Create a message mediator for the alexaext:Extensions registered with given alexaext::ExtensionProvider.
     * @param provider The extension provider.
     * @param messageExecutor Process an extension message in a manner consistent with the APL execution model.
     */
    static ExtensionMediatorPtr
    create(const alexaext::ExtensionProviderPtr& provider, const alexaext::ExecutorPtr& messageExecutor) {
        return std::make_shared<ExtensionMediator>(provider, messageExecutor);
    }

    /**
     * Register the extensions found in the associated  alexaext::ExtensionProvider.  Must be called before
     * RootContext::create();
     *
     * This experimental method will be eliminated when the APL engine can directly process registration
     * messages.
     * @param rootConfig The RootConfig receiving the registered extensions.
     * @param content The document content, contains requested extensions and extension settings.
     * @param uris The URIs requested by the APL document.
     */
    void loadExtensions(const RootConfigPtr& rootConfig, const ContentPtr& content);

    /**
     * Process an extension event. The extension must be registered in the associated alexaext::ExtensionProvider.
     * This experimental method will be eliminated when the APL engine can directly send messages
     * to the extension.
     * @param even The event with type kEventTypeExtension.
     * @param root The root context.
     * @return true if the command was invoked.
     */
    bool invokeCommand(const Event& even);

    /**
     * Use create(...)
     */
    explicit ExtensionMediator(const alexaext::ExtensionProviderPtr& provider,
                               const alexaext::ExecutorPtr& messageExecutor)
            : mProvider(provider),
            mMessageExecutor(messageExecutor) {}

    /**
     * Destructor.
     */
    ~ExtensionMediator() {
        mClients.clear();
    }

private:
    friend class RootContext;

    /**
     * Associate a RootContext to the mediator for event and live data updates.
     */
    void bindContext(const RootContextPtr& context);

    /**
     * Registers the extensions found in the ExtensionProvider by calling RootConfig::registerExtensionXXX().
     */
    void registerExtension(const std::string& uri, const alexaext::ExtensionProxyPtr& extension,
                           const ExtensionClientPtr& client);

    /**
     * Enqueue a message with the executor in response to an extension callback.
     */
    void enqueueResponse(const std::string& uri, const rapidjson::Value& message);

    /**
     * Forward a message to the extension client for processing.
     * @return true if the message was processed.
     */
    void processMessage(const std::string& uri, JsonData&& message);

    // access to the extensions
    std::weak_ptr<alexaext::ExtensionProvider> mProvider;
    // context the context that events and data updates are forwarded to
    std::weak_ptr<RootContext> mRootContext;
    // retro extension wrapper used for message passing
    std::map<std::string, std::shared_ptr<ExtensionClient>> mClients;
    // executor to enqueue/sequence message processing
    alexaext::ExecutorPtr mMessageExecutor;
};

} //namespace apl

#endif //_APL_EXTENSION_MEDIATOR_H
#endif //ALEXAEXTENSIONS
